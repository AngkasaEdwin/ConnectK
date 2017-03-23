#ifndef MOVE_H
#define MOVE_H

#pragma once
class Move
{
public:
	int row; //the row to move to. 
	int col; //the col to move to.
	Move();
	Move(int col, int row);
	bool operator ==(const Move &m) const;
	bool noMove();
	void setMove(int col, int row);
	int getMoveCol();
	int getMoveRow();
};

#endif //MOVE_H
