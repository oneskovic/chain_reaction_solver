#include <SFML/Graphics.hpp>
#include "Header files/GameBoard.h"
#include <iostream>
int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
    GameBoard game_board = GameBoard(10, 10);

    int player = 0;
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
                    if (game_board.PlayAt(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), player))
                        player ^= 1;
                }
            }
        }

        window.clear(sf::Color::White);
        game_board.DisplayBoard(&window);
        window.display();
    }

    return 0;
}