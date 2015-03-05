#ifndef _TRANSPOSITION_NODE_H_
#define _TRANSPOSITION_NODE_H_

#include "Enums.h"

namespace BixNix
{
    
class TranspositionNode
{
public:
    ZobristNumber _hash;
    unsigned int _depth;
    float _score;
    // node type
};

}


#endif // _TRANSPOSITION_NODE_H_
