#include "..\Header files\GameBoard.h"

GameBoard::GameBoard(int no_columns, int no_rows)
{
	board = std::vector<std::vector<GameBoardSquare>>(no_rows);
	for (int row = 0; row < no_columns; row++)
	{
		board[row] = std::vector<GameBoardSquare>(no_columns);
		for (int column = 0; column < no_columns; column++)
		{
			int max_allowed = 3;
			if (row == 0 || row == no_rows-1)
				max_allowed--;
			if (column == 0 || column == no_columns-1)
				max_allowed--;
			board[row][column].max_count = max_allowed;
			board[row][column].player = -1;
			board[row][column].count = 0;
		}
	}
	LoadTextures();
}

bool GameBoard::PlayAt(int row, int column, int player)
{
	if (CanPlayAt(row,column,player))
	{
		if (board[row][column].count == board[row][column].max_count)
			Explode(row, column, player);
		else
		{
			AddPiece(row, column, player);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool GameBoard::PlayAt(sf::Vector2f point, int player)
{
	int row = point.y / (vertical_spacing + line_width);
	int column = point.x / (horizontal_spacing + line_width);
	return PlayAt(row,column,player);
}

void GameBoard::DisplayBoard(sf::RenderWindow* window)
{
	line_width = 3;
	vertical_spacing = (window->getSize().x - (board.size() + 1) * line_width) * 1.0 / board.size();
	horizontal_spacing = (window->getSize().y - (board[0].size() + 1) * line_width) * 1.0 / board[0].size();

	double start_y = 0;
	for (int row = 0; row < board.size(); row++)
	{
		double start_x = 0;
		for (int column = 0; column < board[0].size(); column++)
		{
			sf::Sprite figures;
			sf::Texture* texture;
			if (board[row][column].player == 0)
			{
				texture = &player0_textures[board[row][column].count];
			}
			else
			{
				texture = &player1_textures[board[row][column].count];
			}
			figures.setTexture(*texture);

			double ratio_x = 1.0 * vertical_spacing / texture->getSize().x;
			double ratio_y = 1.0 * horizontal_spacing / texture->getSize().y;
			figures.setScale(sf::Vector2f(ratio_x, ratio_y));

			figures.setPosition(start_x, start_y);
			window->draw(figures);

			start_x += vertical_spacing + line_width;
		}
		start_y += horizontal_spacing + line_width;
	}

	/*Draw grid*/

	//Draw vertical lines
	double start_x = 0;
	for (int column = 0; column < board[0].size(); column++)
	{
		sf::RectangleShape line(sf::Vector2f(line_width, window->getSize().y));
		line.setPosition(start_x, 0);
		line.setFillColor(sf::Color::Black);
		window->draw(line);
		start_x += vertical_spacing + line_width;
	}

	//Draw horizontal lines
	start_y = 0;
	for (int row = 0; row < board.size(); row++)
	{
		sf::RectangleShape line(sf::Vector2f(window->getSize().x, line_width));
		line.setPosition(0, start_y);
		line.setFillColor(sf::Color::Black);
		window->draw(line);
		start_y += horizontal_spacing + line_width;
	}
}

bool GameBoard::LoadTextures()
{
	player0_textures = std::vector<sf::Texture>(4);
	player1_textures = std::vector<sf::Texture>(4);
	for (int i = 0; i < 4; i++)
	{
		sf::Texture texture;
		
		if (!texture.loadFromFile("C:/Users/Fizz2/Desktop/chain-reaction/Resources/Red/PNG/Red" + std::to_string(i) + ".png"))
			return false;
		player0_textures[i] = texture;

		if (!texture.loadFromFile("C:/Users/Fizz2/Desktop/chain-reaction/Resources/Green/PNG/Green" + std::to_string(i) + ".png"))
			return false;
		player1_textures[i] = texture;
	}
	return true;
}

bool GameBoard::CanPlayAt(int row, int column, int player)
{
	if (!IsValidSquare(row,column))
		return false;
	return board[row][column].player == -1 || board[row][column].player == player;
}

bool GameBoard::IsValidSquare(int row, int column)
{
	if (row < 0 || row >= board.size() || column < 0 || column >= board[0].size())
		return false;
	return true;
}

void GameBoard::AddPiece(int row, int column, int player)
{
	if (IsValidSquare(row,column))
	{
		board[row][column].player = player;
		board[row][column].count++;
	}
}

void GameBoard::ClearSquare(int row, int column)
{
	board[row][column].player = -1;
	board[row][column].count = 0;
}

void GameBoard::Explode(int row, int column, int player)
{
	std::stack<std::pair<int, int>> to_visit; to_visit.push({ row,column });
	std::vector<std::pair<int, int>> deltas = { {1,0},{-1,0},{0,1},{0,-1} };

	while (!to_visit.empty())
	{
		int current_row = to_visit.top().first;
		int current_column = to_visit.top().second;
		to_visit.pop();

		//Add pieces to neighboring squares
		for (int i = 0; i < deltas.size(); i++)
		{
			int neighbor_row = current_row + deltas[i].first;
			int neighbor_column = current_column + deltas[i].second;
			AddPiece(neighbor_row, neighbor_column, player);
		}

		//Explode all neighbors :)
		for (int i = 0; i < deltas.size(); i++)
		{
			int neighbor_row = current_row + deltas[i].first;
			int neighbor_column = current_column + deltas[i].second;
			if (IsValidSquare(neighbor_row,neighbor_column) &&
				board[neighbor_row][neighbor_column].count > board[neighbor_row][neighbor_column].max_count)
				to_visit.push({ neighbor_row,neighbor_column });
		}

		ClearSquare(current_row, current_column);
	}
}
