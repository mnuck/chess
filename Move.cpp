#include <iostream>

#include "Move.h"

namespace BixNix
{

std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
{
    lhs << int(rhs._source) << " -> " << int(rhs._target) << " promote to: " << int(rhs._piece);
    return lhs;
}

bool Move::operator==(const Move& rhs)
{
    return _source == rhs._source && 
           _target == rhs._target && 
           _piece  == rhs._piece;
}

}
