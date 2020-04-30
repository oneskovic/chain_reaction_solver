#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <stack>
class GameBoard
{
public:
	struct GameBoardSquare
	{
		int player; //Denotes the player who owns the pieces on the square
		int count; //Number of pieces on the square
		int max_count; //Maximum number of pieces allowed on the square
	};
	GameBoard(int no_columns, int no_rows);
	bool PlayAt(int row, int column, int player);
	bool PlayAt(sf::Vector2f point, int player);
	void DisplayBoard(sf::RenderWindow* window);
private:
	bool LoadTextures();

	bool CanPlayAt(int row, int column, int player);
	bool IsValidSquare(int row, int column);
	void AddPiece(int row, int column, int player);
	void ClearSquare(int row, int column);
	void Explode(int row, int column, int player);

	std::vector<sf::Texture> player0_textures;
	std::vector<sf::Texture> player1_textures;
	std::vector<std::vector<GameBoardSquare>> board;
	int line_width,vertical_spacing,horizontal_spacing;

};

