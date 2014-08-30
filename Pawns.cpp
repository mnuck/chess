#include "BitBoard.h"
#include "Board.h"
#include "Pawns.h"

Pawns& Pawns::GetInstance()
{
    static Pawns instance;
    return instance;
}


Pawns::~Pawns()
{
    // nada
}

Pawns::Pawns()
{
    // nada
}


BitBoard Pawns::getAttacksFrom(BitBoard attackers,
                               BitBoard targets,
                               Board::Color color)
{
    int westOffset = 9 - (color << 4);
    int eastOffset = 7 - (color << 4);
    BitBoard westAttacks = attackers & notAFile << westOffset;
    BitBoard eastAttacks = attackers & notHFile << eastOffset;
    return (eastAttacks | westAttacks) & targets;
}


BitBoard Pawns::getMovesFrom(BitBoard pawns,
                             BitBoard blockers,
                             Board::Color color)
{
    int moveOffset = 8 - (color << 4);
    BitBoard oneStep = (pawns << moveOffset) & ~blockers;
    
    BitBoard homeRow = 0x000000000000FF00 << (color * 40);
    BitBoard fwdOne  = 0x0000000000FF0000 << (color * 24);
    
    BitBoard twoStep = 
        ~blockers &
        ((fwdOne & oneStep) << moveOffset) &
        ((homeRow & pawns) << (2 * moveOffset));

    return oneStep & twoStep;
}
