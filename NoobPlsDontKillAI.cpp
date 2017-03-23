//#include "stdafx.h"
#include "NoobPlsDontKillAI.h"
#include <iostream>
#include <cmath>
using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;
using namespace std;

NoobPlsDontKillAI::NoobPlsDontKillAI(int numCols, int numRows, bool gravityOn, int** gameState, Move lastMove)
{
	this->deadline = 0;
	this->numRows = numRows;
	this->numCols = numCols;
	this->gravityOn = gravityOn;
	this->gameState = gameState;
	this->lastMove = lastMove;
}


NoobPlsDontKillAI::~NoobPlsDontKillAI()
{

	//delete the gameState variable.
	for (int i = 0; i<numCols; i++){
		delete[] gameState[i];
	}
	delete[] gameState;

}

Move NoobPlsDontKillAI::makeMove(){
	start_time = get_time::now(); //Start the clock
	std::chrono::steady_clock::time_point end_time = get_time::now();
	std::chrono::duration<double> run_duration = end_time - start_time;
	std::vector<Move> bestPath; // the path to reach the best node
	Move m(-1, -1); //Move to be returned;
	int currMovCol = numCols / 2; //best column in current iteration
	int currMovRow = numRows / 2; //best row in current iteration
	int movCol = currMovCol; //best column
	int movRow = currMovRow; //best row
	int alpha = -999999999;
	int beta = 999999999;
	int x = 0; //indexes for the current best path
	int y = 0;
	int currValue = 0;
	int currBestValue = -1000000000;
	depth_limit = 1;

	if (gravityOn && gameState[movCol][0] == NO_PIECE && k > 3) //If noone takes the bottom middle slot, take it
		movRow = numRows - 1;
	else if (gameState[movCol][movRow] != NO_PIECE || gravityOn || k <= 3){//If noone takes the middle slot, take it
		currBestValue = -1000000000;
		while (std::chrono::duration_cast<ms>(run_duration).count() < (double)deadline - 150.0){ // while there's still time to iterate deeper...
			if (prevBestPath.size() > 1){ //prevBestPath is checked first
				int size = prevBestPath.size();
				x = prevBestPath[prevBestPath.size() - 1].getMoveCol();
				y = prevBestPath[prevBestPath.size() - 1].getMoveRow();
				//If enemy takes the optimal path, search the path first for maximum pruning
				if (x == lastMove.getMoveCol() && y == lastMove.getMoveRow()){
					x = prevBestPath[prevBestPath.size() - 2].getMoveCol();
					y = prevBestPath[prevBestPath.size() - 2].getMoveRow();
					gameState[x][y] = AI_PIECE;
					currValue = minValue(gameState, 1, alpha, beta, bestPath, true);
					gameState[x][y] = NO_PIECE;

					if (currValue > currBestValue){
						currBestValue = currValue;
						alpha = currBestValue;
						currMovCol = x;
						currMovRow = y;
						prevBestPath.clear();
						prevBestPath = bestPath; //the best path beneath the current best child node
					}
				}
			}
			for (int i = 0; i < numCols; i++){
				for (int j = 0; j < numRows; j++){ // search from bottom up
					if (gameState[i][j] == NO_PIECE){ //Check every possible move available
						//return the worst value the opponent can make if the AI choose this action
						gameState[i][j] = AI_PIECE;
						currValue = minValue(gameState, 1, alpha, beta, bestPath);
						gameState[i][j] = NO_PIECE;

						//check for time deadline (5000ms)
						end_time = get_time::now();
						run_duration = end_time - start_time;
						if (std::chrono::duration_cast<ms>(run_duration).count() >= (double)deadline - 150.0){
							//If this is the first iteration and time is up, set move to current iteration's best move
							if (depth_limit == 1)
								m.setMove(currMovCol, currMovRow);
							else
								m.setMove(movCol, movRow); //returns best move from previous iteration
							return m;
						}

						if (currValue > currBestValue){
							currBestValue = currValue;
							alpha = currBestValue;
							currMovCol = i;
							currMovRow = j;
							prevBestPath.clear();
							prevBestPath = bestPath; //the best path beneath the current best child node
						}
						if (gravityOn)
							break; // skip to the next column
					}
				}
			}
			movCol = currMovCol;
			movRow = currMovRow;
			currBestValue = -1000000000;
			alpha = -999999999;
			depth_limit++;
			end_time = get_time::now();
			run_duration = end_time - start_time;
		}
	}

	m.setMove(movCol, movRow);

	return m;

}

//Terminal and Cutoff functions
//Returns true when the game ends
bool NoobPlsDontKillAI::terminal(int** gameState){
	//Check if there's a winner
	if (winCheck(gameState))
		return true;
	//Check if board is full
	for (int i = 0; i < numCols; i++)
		for (int j = 0; j < numRows; j++)
			if (gameState[i][j] == NO_PIECE)
				return false;
	return true; //draw
}

//Returns true when game ends or cutoff depth is reached
bool NoobPlsDontKillAI::cutOff(int** gameState, int depth){
	return (depth >= depth_limit || terminal(gameState));
}

//MINIMAX SEARCH
int NoobPlsDontKillAI::maxValue(int** gameState, int depth, int alpha, int beta, std::vector<Move> &bestPath, bool priority){
	if (cutOff(gameState, depth))
		return evalState(gameState);

	std::vector<Move> tempBestPath; //the best path from the current best child node
	if (priority && prevBestPath.size() >= depth + 2){
		int size = prevBestPath.size();
		int i = prevBestPath[size - (depth + 2)].getMoveCol();
		int j = prevBestPath[size - (depth + 2)].getMoveRow();

		gameState[i][j] = AI_PIECE;
		int tempValue = minValue(gameState, depth + 1, alpha, beta, bestPath, true);
		gameState[i][j] = NO_PIECE;

		//If alpha changes and it's not pruning, update the best path
		if (tempValue > alpha){
			alpha = tempValue;
			if (alpha >= beta)
				return 999999999; //Prune the remaining iterations
			else{
				tempBestPath = bestPath; //the best path beneath the current best child node
				tempBestPath.push_back(Move(i, j)); //add the current best child node to the path
			}
		}
		bestPath = tempBestPath; //pass the current best path upwards by reference
		return alpha;
	}

	for (int i = 0; i < numCols; ++i) {
		for (int j = 0; j < numRows; ++j) {
			if (gameState[i][j] == NO_PIECE){
				gameState[i][j] = AI_PIECE;
				int tempValue = minValue(gameState, depth + 1, alpha, beta, bestPath);
				gameState[i][j] = NO_PIECE;

				//check for time deadline (5000ms)
				std::chrono::steady_clock::time_point end_time = get_time::now();
				std::chrono::duration<double> run_duration = end_time - start_time;
				if (std::chrono::duration_cast<ms>(run_duration).count() >= (double)deadline - 150.0){
					return 999999999; //Do not pick this path since other possibilities are not yet discovered
				}

				//If alpha changes and it's not pruning, update the best path
				if (tempValue > alpha){
					alpha = tempValue;
					if (alpha >= beta)
						return 999999999; //Prune the remaining iterations
					else{
						tempBestPath = bestPath; //the best path beneath the current best child node
						tempBestPath.push_back(Move(i, j)); //add the current best child node to the path
					}
				}

				if (gravityOn)
					break; // skip to the next column
			}
		}
	}
	bestPath = tempBestPath; //pass the current best path upwards by reference
	return alpha;
}

int NoobPlsDontKillAI::minValue(int** gameState, int depth, int alpha, int beta, std::vector<Move> &bestPath, bool priority){
	if (cutOff(gameState, depth))
		return evalState(gameState);

	std::vector<Move> tempBestPath; //the best path from the current best child node
	if (priority && prevBestPath.size() >= depth + 2){
		int size = prevBestPath.size();
		int i = prevBestPath[size - (depth + 2)].getMoveCol();
		int j = prevBestPath[size - (depth + 2)].getMoveRow();
		
		gameState[i][j] = HUMAN_PIECE;
		int tempValue = maxValue(gameState, depth + 1, alpha, beta, bestPath, true);
		gameState[i][j] = NO_PIECE;

		//If beta changes and it's not pruning, update the best path
		if (tempValue < beta){
			beta = tempValue;
			if (alpha >= beta)
				return -999999999; //Prune the remaining iterations
			else{
				tempBestPath = bestPath; //the best path beneath the current best child node
				tempBestPath.push_back(Move(i, j)); //add the current best child node to the path
			}
		}
		bestPath = tempBestPath; //pass the current best path upwards by reference
		return beta;
	}
	for (int i = 0; i < numCols; ++i) {
		for (int j = 0; j < numRows; ++j) {
			if (gameState[i][j] == NO_PIECE){
				gameState[i][j] = HUMAN_PIECE;
				int tempValue = maxValue(gameState, depth + 1, alpha, beta, bestPath);
				gameState[i][j] = NO_PIECE;

				//check for time deadline (5000ms)
				std::chrono::steady_clock::time_point end_time = get_time::now();
				std::chrono::duration<double> run_duration = end_time - start_time;
				if (std::chrono::duration_cast<ms>(run_duration).count() >= (double)deadline - 150.0){
					return -999999999; //Do not pick this path since other possibilities are not yet discovered
				}

				//If beta changes and it's not pruning, update the best path
				if (tempValue < beta){
					beta = tempValue;
					if (alpha >= beta)
						return -999999999; //Prune the remaining iterations
					else{
						tempBestPath = bestPath; //the best path beneath the current best child node
						tempBestPath.push_back(Move(i, j)); //add the current best child node to the path
					}
				}

				if (gravityOn)
					break; // skip to the next column
			}
		}
	}
	bestPath = tempBestPath; //pass the current best path upwards by reference
	return beta;
}

/*Heuristic state evaluation - line value increases exponentially proportional to its length
Performance Measure:
not possible to create k-in-a-row = 0
one piece = 3^1 = 3
two pieces out of k = 3^2 (may be non-continuous line, with holes in between segments)
n pieces out of k = 3^n
k-in-a-row = 999,999,999 (infinity)
multiple (k-1)-in-a-row/ ways to win > 1 = 900,000,000
Note: potential line that is blocked on 1 side scores only half its value
*/
int NoobPlsDontKillAI::evalState(int** gameState){
	int boardValue = 0;
	int almostWinCountA = 0; //having multiple k-1 potential lines is almost as good as winning
	int almostWinCountB = 0; //having opposing multiple k-1 potential lines is almost as bad as losing
	for (int i = 0; i < numCols; ++i) {
		for (int j = 0; j < numRows; ++j) {
			// if the space previous is either not the same as current,
			// empty, or OOB
			// while the next thing is the same AND not OOB
			// increment contiguous count
			// Add or subtract score based on contiguous pieces
			// searches to the right and up

			if (gameState[i][j] == NO_PIECE) {
				if (gravityOn)
					break;// go to next column
				else
					continue;// move up
			}

			int count = 1; //number of tiles-in-a-row (a line), call this main tiles
			int sepCount = 0; //any extra tiles within k-1 tiles that is not directly connected to main tiles
			bool sideBlocked = false; //potential line that is blocked on 1 side scores slightly lower

			//If this is the beginning of the "line"
			if (i - 1 < 0 || gameState[i - 1][j] != gameState[i][j]) { // horizontal
				while (i + count < numCols && gameState[i][j] == gameState[i + count][j])
					++count;
				if (count >= k){
					boardValue = gameState[i][j] * (999999999); //The winning move
					return boardValue;
				}
				else {//check if # of empty slots before and after this "line" is at least (k - count)
					int remSpace = 0;
					int offset = count; //offset from the original tile
					//Conditions:
					//1. Not out of bounds
					//2. Check only up to k-1 tiles next to the original tile i,j
					while (i + offset < numCols && k > count + remSpace + sepCount){ //check the right side
						//slot is occupied, count is broken if it is not current piece
						if (gameState[i + offset][j] == NO_PIECE){
							remSpace++;
						}
						//Opposing piece
						else if (gameState[i + offset][j] != gameState[i][j]){
							if (offset == count)
								sideBlocked = true;
							break;
						}
						//Equal to Current piece
						else{
							sepCount++;
						}
						offset++;
					}
					// out of bounds to the right
					if (i + offset >= numCols)
						sideBlocked = true;
					offset = 1;
					while (i - offset >= 0 && k > count + remSpace + sepCount){ //check the left side
						if (gameState[i - offset][j] != NO_PIECE){
							break; //slot is occupied
						}
						remSpace++;
						offset++;
					}

					if (i-1 < 0 || gameState[i - 1][j] != NO_PIECE)
						sideBlocked = true;
					if (remSpace >= k - count - sepCount){ //check if a k-length line can still be created using the remaining space
						if (count == k - 1){
							if (gameState[i][j] == AI_PIECE){
								almostWinCountA++;
								if (sideBlocked == false)
									almostWinCountA++;
							}
							else{
								almostWinCountB++;
								if (sideBlocked == false)
									almostWinCountB++;
							}
						}
						boardValue += pow(3, count + sepCount) / (sideBlocked? 2 : 1) * gameState[i][j]; //negative value if opponent, positive otherwise
					}
				}
			}

			if (i - 1 < 0 || j - 1 < 0 || gameState[i - 1][j - 1] != gameState[i][j]) { // diagonal
				// (j-1<0) needed to avoid OOB (out of border)
				count = 1;
				sepCount = 0;
				sideBlocked = false;
				while (i + count < numCols && j + count < numRows
					&& gameState[i][j] == gameState[i + count][j + count])
					++count;
				if (count >= k){
					boardValue = gameState[i][j] * (999999999); //The winning move
					return boardValue;
				}
				else {
					//check if # of empty slots before and after this diagonal "line" is at least (k - count)
					int remSpace = 0;
					int offset = count;
					while (i + offset < numCols && j + offset < numRows &&  k > count + remSpace + sepCount){ //check the right&up side
						//slot is occupied, count is broken if it is not current piece
						if (gameState[i + offset][j + offset] == NO_PIECE){
							remSpace++;
						}
						//Opposing piece
						else if (gameState[i + offset][j + offset] != gameState[i][j]){
							if (offset == count)
								sideBlocked = true;
							break;
						}
						//Equal to Current piece
						else{
							sepCount++;
						}
						offset++;
					}
					// out of bounds to the right
					if (i + offset >= numCols || j + offset >= numRows)
						sideBlocked = true;
					offset = 1;
					while (i - offset >= 0 && j - offset >= 0 && k > count + remSpace + sepCount){ //check the left&down side
						if (gameState[i - offset][j - offset] != NO_PIECE)
							break; //slot is occupied
						remSpace++;
						offset++;
					}
					if (i - 1 < 0 || j - 1 < 0 || gameState[i - 1][j - 1] != NO_PIECE)
						sideBlocked = true;
					if (remSpace >= k - count - sepCount){ //check if a k-length line can still be created using the remaining space
						if (count == k - 1){
							if (gameState[i][j] == AI_PIECE){
								almostWinCountA++;
								if (sideBlocked == false)
									almostWinCountA++;
							}
							else{
								almostWinCountB++;
								if (sideBlocked == false)
									almostWinCountB++;
							}
						}
						boardValue += pow(3, count + sepCount) / (sideBlocked ? 2 : 1) * gameState[i][j]; //negative value if opponent, positive otherwise
					}
				}
			}

			if (i - 1 < 0 || j + 1 >= numRows || gameState[i - 1][j + 1] != gameState[i][j]) { // diagonal
				// (j-1<0) needed to avoid OOB (out of border)
				count = 1;
				sepCount = 0;
				sideBlocked = false;
				while (i + count < numCols && j - count >= 0 && gameState[i][j] == gameState[i + count][j - count])
					++count;
				if (count >= k){
					boardValue = gameState[i][j] * (999999999); //The winning move
					return boardValue;
				}
				else {
					//check if # of empty slots before and after this diagonal "line" is at least (k - count)
					int remSpace = 0;
					int offset = count;
					while (i + offset < numCols && j - offset >= 0 && k > count + remSpace + sepCount){ //check the right&down side
						//slot is occupied, count is broken if it is not current piece
						if (gameState[i + offset][j - offset] == NO_PIECE){
							remSpace++;
						}
						//Opposing piece
						else if (gameState[i + offset][j - offset] != gameState[i][j]){
							if (offset == count)
								sideBlocked = true;
							break;
						}
						//Equal to Current piece
						else{
							sepCount++;
						}
						offset++;
					}
					// out of bounds to the right
					if (i + offset >= numCols || j - offset < 0)
						sideBlocked = true;
					offset = 1;
					while (i - offset >= 0 && j + offset < numRows &&  k > count + remSpace + sepCount){ //check the left&up side
						if (gameState[i - offset][j + offset] != NO_PIECE)
							break; //slot is occupied
						remSpace++;
						offset++;
					}
					if (i - 1 < 0 || j + 1 >= numRows || gameState[i - 1][j + 1] != NO_PIECE)
						sideBlocked = true;
					if (remSpace >= k - count - sepCount){ //check if a k-length line can still be created using the remaining space
						if (count == k - 1){
							if (gameState[i][j] == AI_PIECE){
								almostWinCountA++;
								if (sideBlocked == false)
									almostWinCountA++;
							}
							else{
								almostWinCountB++;
								if (sideBlocked == false)
									almostWinCountB++;
							}
						}
						boardValue += pow(3, count + sepCount) / (sideBlocked ? 2 : 1) * gameState[i][j]; //negative value if opponent, positive otherwise
					}
				}
			}

			if (j - 1 < 0 || gameState[i][j - 1] != gameState[i][j]) { // vertical
				count = 1;
				sepCount = 0;
				sideBlocked = false;
				while (j + count < numRows && gameState[i][j] == gameState[i][j + count])
					++count;
				if (count >= k){
					boardValue = gameState[i][j] * (999999999); //The winning move
					return boardValue;
				}
				else {//check if # of empty slots before and after this "line" is at least (k - count)
					int remSpace = 0;
					int offset = count;
					while (j + offset < numRows &&  k > count + remSpace + sepCount){ //check above
						//slot is occupied, count is broken if it is not current piece
						if (gameState[i][j + offset] == NO_PIECE){
							remSpace++;
						}
						//Opposing piece
						else if (gameState[i][j + offset] != gameState[i][j]){
							if (offset == count)
								sideBlocked = true;
							break;
						}
						//Equal to Current piece
						else{
							sepCount++;
						}
						offset++;
					}
					// out of bounds to the right
					if (j + offset >= numRows)
						sideBlocked = true;
					offset = 1;
					while (j - offset >= 0 && k > count + remSpace + sepCount){ //check below
						if (gameState[i][j - offset] != NO_PIECE)
							break; //slot is occupied
						remSpace++;
						offset++;
					}
					if (j - 1 < 0 || gameState[i][j - 1] != NO_PIECE)
						sideBlocked = true;
					if (remSpace >= k - count - sepCount){ //check if a k-length line can still be created using the remaining space
						if (count == k - 1){
							if (gameState[i][j] == AI_PIECE){
								almostWinCountA++;
								if (sideBlocked == false)
									almostWinCountA++;
							}
							else{
								almostWinCountB++;
								if (sideBlocked == false)
									almostWinCountB++;
							}
						}
						boardValue += pow(3, count + sepCount) / (sideBlocked ? 2 : 1) * gameState[i][j]; //negative value if opponent, positive otherwise
					}
				}
			}
		}
	}
	//multiple ways of winning/losing
	if (almostWinCountB >= 2)
		boardValue = -900000000;
	else if (almostWinCountA >= 2)
		boardValue = 900000000;
	return boardValue;
}

//Win check
bool NoobPlsDontKillAI::winCheck(int** gameState){
	for (int i = 0; i < numCols; ++i) {
		for (int j = 0; j < numRows; ++j) {
			// if the space previous is either not the same as current,
			// empty, or OOB
			// while the next thing is the same AND not OOB
			// increment contiguous count
			// if count greater than or equal to k, return true (there's a winner)
			// searches to the right and up

			if (gameState[i][j] == 0) {
				if (gravityOn)
					break;// go to next column
				else
					continue;// move up
			}

			if (i - 1 < 0 || gameState[i - 1][j] != gameState[i][j]) { // horizontal
				int count = 1;
				while (i + count < numCols && gameState[i][j] == gameState[i + count][j]) {
					++count;
					if (count >= k)
						return true;
				}
			}

			if (i - 1 < 0 || j - 1 < 0 || gameState[i - 1][j - 1] != gameState[i][j]) { // diagonal,
				// (j-1<0)
				// needed
				// to
				// avoid
				// OOB
				int count = 1;
				while (i + count < numCols && j + count < numRows
					&& gameState[i][j] == gameState[i + count][j + count]) {
					++count;
					if (count >= k)
						return true;
				}
			}

			if (i - 1 < 0 || j + 1 >= numRows || gameState[i - 1][j + 1] != gameState[i][j]) { // diagonal,
				// (j+1>=numRows)
				// needed
				// to
				// avoid
				// OOB
				int count = 1;
				while (i + count < numCols && j - count >= 0 && gameState[i][j] == gameState[i + count][j - count]) {
					++count;
					if (count >= k)
						return true;
				}
			}

			if (j - 1 < 0 || gameState[i][j - 1] != gameState[i][j]) { // vertical
				int count = 1;
				while (j + count < numRows && gameState[i][j] == gameState[i][j + count]) {
					++count;
					if (count >= k)
						return true;
				}
			}
		}
	}
	return false;
}