#ifndef _TRANSPOSITION_NODE_H_
#define _TRANSPOSITION_NODE_H_

#include "Enums.h"
#include "Move.h"

namespace BixNix
{
    
class TranspositionNode
{
public:
    enum EvalType
    {
        Alpha,
        Beta,
        Actual
    };

    TranspositionNode() :
        _hash(0LL),
        _depth(0),
        _score(0.0),
        _move(Move(0,0)),
        _evalType(Actual) 
        {}

    ZobristNumber _hash;
    unsigned int _depth;
    int _score;
    Move _move;    

    
    EvalType _evalType;
};

}


#endif // _TRANSPOSITION_NODE_H_
