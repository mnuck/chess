#ifndef _KINGS_H_
#define _KINGS_H_

#include "BitBoard.h"
#include "Board.h"

namespace BixNix
{

class Kings
{
public:
    static Kings& GetInstance();
    virtual ~Kings();
    BitBoard getAttacksFrom(Square kingSquare);
    BitBoard getAttacksFrom(BitBoard king,
                            BitBoard obstructions);
protected:
    Kings();
    BitBoard generateOneStepsFrom(Square index);
    std::array<BitBoard, 64> _attacks;

};

}

#endif // _KINGS_H_
