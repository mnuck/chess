#ifndef _KINGS_H_
#define _KINGS_H_

#include "BitBoard.h"
#include "Board.h"

class Kings
{
public:
    static Kings& GetInstance();
    virtual ~Kings();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard safe);
protected:
    Kings();
};

#endif // _KINGS_H_
