//#include "stdafx.h"
#include "Move.h"

Move::Move()
{
	row = 0;
	col = 0;
}
Move::Move(int col, int row){
	this->row=row;
	this->col=col;
}
bool Move::operator==(const Move &m) const{
	if (this->col == m.col && this->row == m.row)
		return true;
	return false;
}
bool Move::noMove(){
	return (this->row == -1 && this->col == -1);
}
void Move::setMove(int col, int row){
	this->col = col;
	this->row = row;
}

int Move::getMoveCol(){
	return col;
}

int Move::getMoveRow(){
	return row;
}