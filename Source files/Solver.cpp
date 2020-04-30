#include "..\Header files\Solver.h"
#include <iostream>

Solver::Solver(GameBoard* board, int player)
{
	this->board = board;
	this->player = player;
}

std::pair<int, int> Solver::NextMove()
{
	auto squares = board->GetSquares();
	int no_rows = squares.size();
	int no_columns = squares[0].size();

	int best_row = -1;
	int best_column = -1;
	int best_score = player == 0 ? INT_MIN : INT_MAX;

	// Find best possible score for current depth
	for (int row = 0; row < no_rows; row++)
	{
		for (int column = 0; column < no_columns; column++)
		{
			if (board->CanPlayAt(row, column, player))
			{
				auto temp_squares = board->GetSquares();
				board->PlayAt(row, column, player);
				if (player == 0)
				{
					int possible_move_score = Minimax(1);
					if (possible_move_score > best_score)
					{
						best_score = possible_move_score;
						best_row = row;
						best_column = column;
					}
				}
				else
				{
					int possible_move_score = Minimax(0);
					if (possible_move_score < best_score)
					{
						best_score = possible_move_score;
						best_row = row;
						best_column = column;
					}
				}
				board->ReplaceBoard(&temp_squares);
			}
		}
	}

	return std::pair<int, int>(best_row, best_column);
}

int Solver::HeuristicScore()
{
	return board->NumberOfPlayers().first - board->NumberOfPlayers().second;
}

int Solver::Minimax(int player, int depth)
{
	if (board->IsFinished() || depth == 0)
	{
		return HeuristicScore();
	}

	auto squares = board->GetSquares();
	int no_rows = squares.size();
	int no_columns = squares[0].size();

	int best_score = player == 0 ? INT_MIN : INT_MAX;
	// Find best possible score for current depth
	for (int row = 0; row < no_rows; row++)
	{
		for (int column = 0; column < no_columns; column++)
		{
			if (board->CanPlayAt(row, column, player))
			{
				auto temp_squares = board->GetSquares();
				board->PlayAt(row, column, player);
				if (player == 0)
					best_score = std::max(best_score, Minimax(1, depth - 1));
				else
					best_score = std::min(best_score, Minimax(0, depth - 1));
				board->ReplaceBoard(&temp_squares); 
			}
		}
	}

	//std::cout << "Depth: " << depth << " Best score: " << best_score << "\n";
	return best_score;
}
