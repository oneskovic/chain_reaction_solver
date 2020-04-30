#pragma once
#include "GameBoard.h"

class Solver 
{
public:
	Solver(GameBoard* board, int player);
	std::pair<int, int> NextMove();
	
private:
	int HeuristicScore();
	int Minimax(int player = 0, int depth = 3);

	GameBoard* board;
	int player;
};