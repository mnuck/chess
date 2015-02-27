#ifndef _QUEENS_H_
#define _QUEENS_H_

#include "BitBoard.h"
#include "Board.h"

namespace BixNix
{

class Queens
{
public:
    static Queens& GetInstance();
    virtual ~Queens();
    BitBoard getAttacksFrom(const BitBoard& attackers,
                            const BitBoard& targets,
                            const BitBoard& obstructions);

protected:
    Queens();
};

}

#endif // _QUEENS_H_
