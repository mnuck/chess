#ifndef _ROOKS_H_
#define _ROOKS_H_

#include "BitBoard.h"
#include "Board.h"

class Rooks
{
public:
    static Rooks& GetInstance();
    virtual ~Rooks();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard targets,
                            BitBoard friendlies);

protected:
    Rooks();
};

#endif // _ROOKS_H_
