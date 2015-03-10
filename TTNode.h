#ifndef _TTNODE_H_
#define _TTNODE_H_

#include <climits>

#include "Enums.h"
#include "Move.h"

namespace BixNix
{

class TTNode
{
public:
    enum Bound
    {
        Exact,
        Lower,
        Upper
    };

    TTNode() :
        _hash(FFFFFFFFFFFFFFFFLL),
        _score(0),
        _depth(0),
        _bound(Exact) {}

    ZobristNumber _hash;
    int _score;
    unsigned int _depth;
    Bound _bound;
};

}


#endif // _TTNODE_H_
