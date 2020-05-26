#include <SFML/Graphics.hpp>
#include "Header files/GameBoard.h"
#include "Header files/Solver.h"
#include "Header files/NetworkHandler.h"
#include "Header files/NetworkCodes.h"
#include "Header files/Server.h"
#include <iostream>
#include <thread>
#include <random>

std::string GenerateRandomGameID(int length = 5)
{
    static auto& chrs = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while (length--)
        s += chrs[pick(rg)];

    return s;
}

int main()
{
    // Mode 0: Player vs Player
    // Mode 1: Player vs Computer
    // Mode 2: Computer vs Computer
    // Mode 3: Player vs Player (online)
    // Mode 4: Host a server
    int mode;
    std::cin >> mode;
    if (mode == 4)
    {
        Server s = Server();
    }
    else
    {
        sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
        GameBoard game_board = GameBoard(5, 5);

        int player = -1;
        Solver computer1 = Solver(&game_board, 0);
        Solver computer2 = Solver(&game_board, 1);

        sf::TcpSocket socket;
        std::string ip = "192.168.0.15";
        sf::Packet request_packet, response_packet;
        bool isPlayersTurn = false;
        bool isGameStarted = false;
        if (mode == 3)
        {
            std::cout << "Make a room or join an existing one?(type MAKE or JOIN): ";
            std::string response;
            std::cin >> response;
            std::cout << "\n";

            request_packet << REQUEST_JOINGAMEID;
            if (response == "MAKE")
            {
                std::string game_id = GenerateRandomGameID();
                request_packet << game_id;
                std::cout << "Your game id: " << game_id << "\n";
            }
            else
            {
                std::cout << "Enter room id: ";
                std::string input_game_id;
                std::cin >> input_game_id;
                request_packet << input_game_id;
            }

            try
            {
                sf::Socket::Status status = socket.connect(ip, 12000, sf::seconds(5));
                if (status != sf::Socket::Done)
                {
                    throw NetworkException("Error connecting to server");
                }
                std::cout << "Successfully connected to server " << ip << "\n";

                socket.setBlocking(false);
                socket.send(request_packet);
                socket.receive(response_packet);
            }
            catch (NetworkException exception)
            {
                std::cout << exception.what();
            }
        }
        else if (mode != 4) 
        {
            player = 0;
        }

        while (window.isOpen())
        {
            if (mode == 3 && socket.Done)
            {
                int response_type;
                response_packet >> response_type;
                switch (response_type)
                {
                case REQUEST_WAIT:
                    player = 0;
                    isPlayersTurn = true;
                    socket.receive(response_packet);
                case REQUEST_START:
                    if (player == -1) 
                    {
                        player = 1;
                        socket.receive(response_packet);
                    }
                    isGameStarted = true;
                case REQUEST_PLAYMOVE:
                    double x, y;
                    response_packet >> x >> y;
                    game_board.PlayAt(sf::Vector2f(x, y), player ^ 1);
                    isPlayersTurn = !isPlayersTurn;
                default:
                    break;
                }
            }
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        if (mode == 0)
                        {
                            if (game_board.PlayAt(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), player) && !game_board.IsFinished())
                                player ^= 1;
                        }

                        else if (mode == 1)
                        {
                            if (!game_board.IsFinished() && game_board.PlayAt(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), player))
                            {
                                window.clear(sf::Color::White);
                                game_board.DisplayBoard(&window);
                                window.display();
                                std::this_thread::sleep_for(std::chrono::seconds(1));

                                auto computer_move = computer2.NextMove();
                                int x = computer_move.first;
                                int y = computer_move.second;
                                std::cout << "Computer plays at: " << x << " " << y << "\n";
                                game_board.PlayAt(x, y, player ^ 1);
                            }
                        }

                        else if (mode == 2)
                        {
                            if (!game_board.IsFinished())
                            {
                                auto computer1_move = computer1.NextMove();
                                int x1 = computer1_move.first;
                                int y1 = computer1_move.second;
                                std::cout << "Computer1 plays at: " << x1 << " " << y1 << "\n";
                                game_board.PlayAt(x1, y1, 0);
                            }

                            window.clear(sf::Color::White);
                            game_board.DisplayBoard(&window);
                            window.display();
                            std::this_thread::sleep_for(std::chrono::seconds(1));

                            if (!game_board.IsFinished())
                            {
                                auto computer2_move = computer2.NextMove();
                                int x2 = computer2_move.first;
                                int y2 = computer2_move.second;
                                std::cout << "Computer2 plays at: " << x2 << " " << y2 << "\n";
                                game_board.PlayAt(x2, y2, 1);
                            }
                        }
                        
                        else if (mode == 3) 
                        {
                            if (isGameStarted && isPlayersTurn && game_board.PlayAt(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), player)
                                && !game_board.IsFinished())
                            {
                                isPlayersTurn = !isPlayersTurn;

                                request_packet << REQUEST_PLAYMOVE;
                                request_packet << event.mouseButton.x;
                                request_packet << event.mouseButton.y;
                                socket.send(request_packet);

                                socket.receive(response_packet);
                            }
                        }
                    }
                }
            }

            window.clear(sf::Color::White);
            game_board.DisplayBoard(&window);
            window.display();
        }
    }
    

    return 0;
}