#include <SFML/Graphics.hpp>
#include "Header files/GameBoard.h"
#include "Header files/Solver.h"
#include <iostream>
#include <thread>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chain Reaction solver");
    GameBoard game_board = GameBoard(5, 5);

    int player = 0;
    Solver computer1 = Solver(&game_board, 0);
    Solver computer2 = Solver(&game_board, 1);

    // Mode 0: Player vs Player
    // Mode 1: Player vs Computer
    // Mode 2: Computer vs Computer
    int mode = 2;

    while (window.isOpen())
    {
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

                            auto computer_move = computer2.NextMoveMCTS();
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
                            auto computer1_move = computer1.NextMoveMCTS(30000);
                            int x1 = computer1_move.first;
                            int y1 = computer1_move.second;
                            std::cout << "MCTS plays at: " << x1 << " " << y1 << "\n";
                            game_board.PlayAt(x1, y1, 0);
                        }

                        window.clear(sf::Color::White);
                        game_board.DisplayBoard(&window);
                        window.display();

                        if (!game_board.IsFinished())
                        {
                            auto computer2_move = computer2.NextMove();
                            int x2 = computer2_move.first;
                            int y2 = computer2_move.second;
                            std::cout << "Minimax plays at: " << x2 << " " << y2 << "\n";
                            game_board.PlayAt(x2, y2, 1);
                        }

                        window.clear(sf::Color::White);
                        game_board.DisplayBoard(&window);
                        window.display();
                    }
                }
            }
        }

        window.clear(sf::Color::White);
        game_board.DisplayBoard(&window);
        window.display();
    }

    return 0;
}