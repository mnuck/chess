#ifndef _QUEENS_H_
#define _QUEENS_H_

#include "BitBoard.h"
#include "Board.h"

class Queens
{
public:
    static Queens& GetInstance();
    virtual ~Queens();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard targets,
                            BitBoard friendlies);

protected:
    Queens();
};

#endif // _QUEENS_H_
