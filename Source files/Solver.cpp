#include "..\Header files\Solver.h"
#include <iostream>
const double Solver::exploration_constant = sqrt(2);

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
std::pair<int, int> Solver::NextMoveMCTS(int iterations)
{
	GameTree game_tree = GameTree(board,player);
	for (int i = 0; i < iterations; i++)
	{
		if (i % 1000 == 0)
		{
			std::cout << "                                                       ";
			std::cout << "\rIteration: " << i;
		}
		game_tree.RunIteration(player);
	}
	std::cout << "\rIteration: " << iterations << "\n";
	return game_tree.GetBestMove();
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

Solver::GameTree::GameTreeNode::GameTreeNode()
{
	is_leaf = true;
	player = -1;
	wins = 0;
	visits = 0;
	action_to_reach_node.column = -1;
	action_to_reach_node.row = -1;
}

Solver::GameTree::GameTreeNode::GameTreeNode(int player, Solver::GameTree::GameAction action_taken_to_reach)
{
	is_leaf = true;
	this->player = player;
	wins = 0;
	visits = 0;

	action_to_reach_node.column = action_taken_to_reach.column;
	action_to_reach_node.row = action_taken_to_reach.row;
}



double Solver::GameTree::GameTreeNode::GetUCT1()
{
	if (visits == 0)
	{
		return INFINITY;
	}
	int parent_visits = parent->visits;
	double UCT1 = (wins * 1.0 / visits) + exploration_constant * sqrt(log(parent_visits) / visits);
	return UCT1;
}

Solver::GameTree::GameTree(GameBoard* game_board, int solve_as_player)
{
	copy_board = *game_board;
	root.player = (solve_as_player + 1) % 2;
	random_generator = Random();
}

std::pair<int, int> Solver::GameTree::GetBestMove()
{
	int max_wins = -1;
	GameAction best_action = {-1,-1};
	
	//For debugging
	std::cout << "Expected win rates:\n";
	std::vector<std::vector<double>> win_rates = std::vector<std::vector<double>>(copy_board.GetRowCount());
	for (int i = 0; i < win_rates.size(); i++)
	{
		win_rates[i] = std::vector<double>(copy_board.GetRowCount());
	}

	for (auto i = root.children.begin(); i != root.children.end(); i++)
	{
		//For debugging
		double win_rate = (*i)->wins * 1.0 / (*i)->visits;
		GameAction action = (*i)->action_to_reach_node;
		win_rates[action.row][action.column] = win_rate;

		if ((*i)->wins > max_wins)
		{
			best_action = (*i)->action_to_reach_node;
			max_wins = (*i)->wins;
		}
	}

	//For debugging
	for (int i = 0; i < win_rates.size(); i++)
	{
		for (int j = 0; j < win_rates[0].size(); j++)
		{
			printf("%.2f ", win_rates[i][j]);
		}
		std::cout << "\n";
	}

	return std::pair<int, int>(best_action.row, best_action.column);
}

void Solver::GameTree::RunIteration(int player)
{
	GameTreeNode* current_node = &root;
	while (!current_node->is_leaf)
	{
		//Find child that maximizes UCT1 value
		GameTreeNode* max_uct1_child = nullptr;
		double max_uct1 = -INFINITY;
		
		//Node is terminal and has no children
		if (current_node->children.size() == 0)
		{
			break;
		}

		for (auto i = current_node->children.begin(); i != current_node->children.end(); i++)
		{
			double child_uct1 = (*i)->GetUCT1();
			if (child_uct1 > max_uct1)
			{
				max_uct1_child = *i;
				max_uct1 = child_uct1;
			}
		}
		current_node = max_uct1_child;
	}

	//Current node is now a leaf node

	if (current_node->visits == 0) //Node was never visited
	{
		int winning_player = RolloutSimulation(current_node);
		PropagateUpwards(current_node, winning_player);
	}
	else //Node was visited but not expanded
	{
		//Node is terminal, no expansion will occur
		if (copy_board.IsFinished())
		{
			int winning_player = RolloutSimulation(current_node);
			PropagateUpwards(current_node, winning_player);
			current_node->is_leaf = false;
			return;
		}

		//Expand the node
		for (int row = 0; row < copy_board.GetRowCount(); row++)
		{
			for (int col = 0; col < copy_board.GetColumnCount(); col++)
			{
				int other_player = (current_node->player + 1) % 2;
				if (copy_board.CanPlayAt(row, col, other_player))
				{
					GameAction action_taken = { row, col };
					GameTreeNode* new_node = new GameTreeNode(other_player, action_taken);
					new_node->parent = current_node;
					current_node->children.push_back(new_node);
				}
			}
		}
		int random_child_index = random_generator.GetInt(0,current_node->children.size()-1);
		auto child_iterator = current_node->children.begin();
		for (int i = 0; i < random_child_index; i++)
			child_iterator++;

		GameTreeNode* random_child = *child_iterator;
		int winning_player = RolloutSimulation(random_child);
		PropagateUpwards(random_child, winning_player);
		current_node->is_leaf = false;
	}
}

int Solver::GameTree::RolloutSimulation(GameTreeNode* node)
{
	GameBoard actual_board = copy_board;
	GameTreeNode* current_node = node;
	int next_player = (node->player + 1) % 2;

	while (current_node != &root)
	{
		copy_board.PlayAt(current_node->action_to_reach_node.row, current_node->action_to_reach_node.column, current_node->player);
		current_node = current_node->parent;
	}

	while (!copy_board.IsFinished())
	{
		std::vector<GameAction> possible_actions; possible_actions.reserve(copy_board.GetRowCount()* copy_board.GetColumnCount());
		for (int row = 0; row < copy_board.GetRowCount(); row++)
		{
			for (int col = 0; col < copy_board.GetColumnCount(); col++)
			{
				if (copy_board.CanPlayAt(row, col, next_player))
				{
					possible_actions.push_back({ row,col });
				}
			}
		}
		GameAction rand_action = possible_actions[random_generator.GetInt(0, possible_actions.size() - 1)];
		copy_board.PlayAt(rand_action.row, rand_action.column, next_player);
		next_player = (next_player + 1) % 2;
	}

	int winning_player = -1;
	if (copy_board.NumberOfPlayers().first == 0)
		winning_player = 0;
	else
		winning_player = 1;

	copy_board = actual_board;
	return winning_player;
}
void Solver::GameTree::PropagateUpwards(GameTreeNode* start_node, int winning_player)
{
	GameTreeNode* current_node = start_node;
	while (true)
	{
		current_node->visits++;
		if (current_node->player == winning_player)
			current_node->wins++;
		if (current_node == &root)
			break;
		
		current_node = current_node->parent;
	}
}


