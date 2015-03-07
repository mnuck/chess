#include <iostream>

#include "Move.h"

namespace BixNix
{

std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
{
    lhs << int(rhs._source) << " -> " << int(rhs._target);
    switch (rhs._piece)
    {
    case Knight:
        lhs << " promote to Knight";
        break;
    case Rook:
        lhs << " promote to Rook";
        break;
    case Bishop:
        lhs << " promote to Bishop";
        break;
    case Queen:
        break;
    case King:
        lhs << " promote to a King? You don't vote for kings!";
        break;
    case Pawn:
        lhs << " promote to a...Pawn? What does that even mean?";
        break;
    default:
        lhs << " promote to a...wait, what?";
        break;
    }

    return lhs;
}

bool Move::operator==(const Move& rhs) const
{
    return _source == rhs._source && 
           _target == rhs._target && 
           _piece  == rhs._piece;
}

}
