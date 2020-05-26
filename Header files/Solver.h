#pragma once
#include "GameBoard.h"
#include "Random.h"
#include <list>

class Solver 
{
public:
	Solver(GameBoard* board, int player);
	std::pair<int, int> NextMove();
	std::pair<int, int> NextMoveMCTS(int iterations = 10000);
private:
	class GameTree
	{
	public:
		GameTree(GameBoard* game_board, int solve_as_player);
		struct GameAction
		{
			int row, column;
		};
		std::pair<int, int> GetBestMove();
		void RunIteration(int player);
	private:
		Random random_generator;
		GameBoard copy_board;
		class GameTreeNode {
		public:
			GameTreeNode();
			GameTreeNode(int player, Solver::GameTree::GameAction action_taken_to_reach);
			double GetUCT1();
		private:
			GameAction action_to_reach_node;
			bool is_leaf;
			GameTreeNode* parent;
			std::list<GameTreeNode*> children;
			int player, wins, visits;
			friend class GameTree;
		};
		GameTreeNode root;

		/// <summary>
		/// Returns the winning player (0 or 1)
		/// </summary>
		/// <param name="node">Node from which to rollout the simulation</param>
		int RolloutSimulation(GameTreeNode* node);
		void PropagateUpwards(GameTreeNode* start_node, int winning_player);
	};

	int HeuristicScore();
	int Minimax(int alpha = INT_MIN, int beta = INT_MAX, int player = 0, int depth = 5);

	GameBoard* board;
	int player;
	static const double exploration_constant;
};