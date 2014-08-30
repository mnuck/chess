#include "BitBoard.h"
#include "Board.h"
#include "Kings.h"

Kings& Kings::GetInstance()
{
    static Kings instance;
    return instance;
}


Kings::~Kings()
{
    // nada
}

Kings::Kings()
{
    // nada
}


BitBoard Kings::getAttacksFrom(BitBoard kings,
                               BitBoard safe)
{
    int eastOffset = 9 - (color * 16);
    int westOffset = 7 - (color * 16);
    BitBoard eastAttacks = attackers & notAFile << eastOffset;
    BitBoard westAttacks = attackers & notHFile << westOffset;
    return (eastAttacks | westAttacks) & targets;
}
