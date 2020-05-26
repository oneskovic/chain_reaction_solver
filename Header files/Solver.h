#pragma once
#include "GameBoard.h"

class Solver 
{
public:
	Solver(GameBoard* board, int player);
	std::pair<int, int> NextMove();
	
private:
	int HeuristicScore();
	int Minimax(int alpha = INT_MIN, int beta = INT_MAX, int player = 0, int depth = 5);


	GameBoard* board;
	int player;
};