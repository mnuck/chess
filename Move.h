#ifndef __MOVE_H__
#define __MOVE_H__

#include <iostream>

#include "BitBoard.h"

class Move
{
public:
    Move(Square source, Square target): _source(source), _target(target) {}
    Square _source;
    Square _target;
};

std::ostream& operator<<(std::ostream& lhs, const Move& rhs);


#endif // __MOVE_H_
