#ifndef NOOBPLSDONTKILLAI_H
#define NOOBPLSDONTKILLAI_H

#pragma once
#include "Move.h"
#include <cstdlib>
#include <vector>
#include <chrono>


// A new AIShell will be created for every move request.
class NoobPlsDontKillAI{

public:
	//these represent the values for each piece type.
	static const int AI_PIECE=1;
	static const int HUMAN_PIECE = -1;
	static const int NO_PIECE=0;

private:
	//Do not alter the values of numRows or numcols.
	//they are used for deallocating the gameState variable.
	int numRows; //the total number of rows in the game state.
	int numCols; //the total number of columns in the game state.
	int **gameState; //a pointer to a two-dimensional array representing the game state.
	bool gravityOn; //this will be true if gravity is turned on. It will be false if gravity is turned off.
	Move lastMove; //this is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.
	int depth_limit;
	std::vector<Move> prevBestPath;
	std::chrono::steady_clock::time_point start_time;

public:
	int deadline; //this is how many milliseconds the AI has to make move.
	int k;        // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

	NoobPlsDontKillAI(int numCols, int numRows, bool gravityOn, int** gameState, Move lastMove);
	~NoobPlsDontKillAI();
	Move makeMove();
	//Terminal and Cutoff functions
	bool terminal(int** gameState);
	bool cutOff(int** gameState, int depth);
	//Minimax search
	int maxValue(int** gameState, int depth, int alpha, int beta, std::vector<Move> &bestPath, bool priority = false);
	int minValue(int** gameState, int depth, int alpha, int beta, std::vector<Move> &bestPath, bool priority = false);
	//State evaluation heuristic
	int evalState(int** gameState);
	//Check if there's a winner
	bool winCheck(int** gameState);
	//check if any Move on vector v is occupied
};

#endif //AISHELL_H
