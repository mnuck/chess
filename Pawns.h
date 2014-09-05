#ifndef _PAWNS_H_
#define _PAWNS_H_

#include "BitBoard.h"
#include "Board.h"

class Pawns
{
public:
    static Pawns& GetInstance();
    virtual ~Pawns();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard targets,
                            Color color);
    BitBoard getMovesFrom(BitBoard pawns,
                          BitBoard blockers,
                          Color color);

protected:
    Pawns();
};

#endif // _PAWNS_H_
