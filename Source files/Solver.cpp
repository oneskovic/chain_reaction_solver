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

	int alpha = INT_MIN;
	int beta = INT_MAX;
	int depth = 5;

	// Repeat minimax with smaller depth if current depth determines that it cannot win
	while (best_row == -1 && best_column == -1 && depth >= 0)
	{
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
						int possible_move_score = Minimax(alpha, beta, 1, depth);
						if (possible_move_score > best_score)
						{
							best_score = possible_move_score;
							best_row = row;
							best_column = column;
						}
						alpha = std::max(alpha, best_score);
					}
					else
					{
						int possible_move_score = Minimax(alpha, beta, 0, depth);
						if (possible_move_score < best_score)
						{
							best_score = possible_move_score;
							best_row = row;
							best_column = column;
						}
						beta = std::min(beta, best_score);
					}
					board->ReplaceBoard(&temp_squares);
					// Prune if possible
					if (alpha >= beta)
						goto escape;
				}
			}
		}

	escape:
		depth--;
	}
	return std::pair<int, int>(best_row, best_column);
}

int Solver::HeuristicScore()
{
	// First player tries to maximize this score, second to minimize
	int score = board->NumberOfPlayers().first - board->NumberOfPlayers().second;

	if (!board->IsFinished())
		return score;
	
	else
	{
		if (score > 0)
			return INT_MAX;
		else
			return INT_MIN;
	}
}

int Solver::Minimax(int alpha, int beta, int player, int depth)
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
				{
					best_score = std::max(best_score, Minimax(alpha, beta, 1, depth - 1));
					alpha = std::max(alpha, best_score);
				}
				else
				{
					best_score = std::min(best_score, Minimax(alpha, beta, 0, depth - 1));
					beta = std::min(beta, best_score);
				}

				board->ReplaceBoard(&temp_squares);

				// Prune if possible
				if (alpha >= beta)
					goto escape;
			}
		}
	}

	escape:
	//std::cout << "Depth: " << depth << " Best score: " << best_score << "\n";
	return best_score;
}
