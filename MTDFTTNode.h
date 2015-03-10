#ifndef _MTDF_TTNODE_H_
#define _MTDF_TTNODE_H_

#include <climits>

#include "Enums.h"
#include "Move.h"

namespace BixNix
{
    
class MTDFTTNode
{
public:
    MTDFTTNode() :
        _hash(0xFFFFFFFFFFFFFFFFLL),
        _lower(INT_MIN),
        _upper(INT_MAX),
        _depth(0),
        _move(Move(0,0)) {}

    ZobristNumber _hash;
    int _lower;
    int _upper;
    unsigned int _depth;
    Move _move;
    
};

}


#endif // _TRANSPOSITION_NODE_H_
