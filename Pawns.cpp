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
    int eastOffset = 9 - (color * 16);
    int westOffset = 7 - (color * 16);
    BitBoard eastAttacks = attackers & notAFile << eastOffset;
    BitBoard westAttacks = attackers & notHFile << westOffset;
    return (eastAttacks | westAttacks) & targets;
}
