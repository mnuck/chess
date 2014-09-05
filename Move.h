#ifndef __MOVE_H__
#define __MOVE_H__

#include <iostream>

#include "BitBoard.h"
#include "Enums.h"

class Move
{
public:
    Move(Square source, Square target, Piece piece=Queen): 
        _source(source), _target(target), _piece(piece) {}
    Square _source;
    Square _target;
    Piece _piece;
};

std::ostream& operator<<(std::ostream& lhs, const Move& rhs);


#endif // __MOVE_H_
