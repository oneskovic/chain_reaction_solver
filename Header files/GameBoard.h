#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <stack>
#include <queue>
class GameBoard
{
public:
	struct GameBoardSquare
	{
		int player; //Denotes the player who owns the pieces on the square
		int count; //Number of pieces on the square
		int max_count; //Maximum number of pieces allowed on the square
	};

	GameBoard(int no_columns = 10, int no_rows = 10);
	bool PlayAt(int row, int column, int player);
	bool PlayAt(sf::Vector2f point, int player);
	void DisplayBoard(sf::RenderWindow* window);
	bool CanPlayAt(int row, int column, int player);
	bool IsFinished();
	std::pair<int, int> NumberOfPlayers();
	std::vector<std::vector<GameBoardSquare>> GetSquares();
	void ReplaceBoard(std::vector<std::vector<GameBoardSquare>>* board);
	
private:
	bool LoadTextures();

	bool IsValidSquare(int row, int column);
	void AddPiece(int row, int column, int player);
	void ClearSquare(int row, int column);
	void Explode(int row, int column, int player);

	std::vector<sf::Texture> player0_textures;
	std::vector<sf::Texture> player1_textures;
	int line_width,vertical_spacing,horizontal_spacing;
	std::vector<std::vector<GameBoardSquare>> board;
};

