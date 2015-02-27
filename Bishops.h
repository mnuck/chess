#ifndef _BISHOPS_H_
#define _BISHOPS_H_

#include "BitBoard.h"

namespace BixNix
{

class Bishops
{
public:
    static Bishops& GetInstance();
    virtual ~Bishops();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard targets,
                            BitBoard friendlies);

protected:
    Bishops();
};

}

#endif // _BISHOPS_H_
