#include "BitBoard.h"
#include "Board.h"
#include "Rooks.h"

Rooks& Rooks::GetInstance()
{
    static Rooks instance;
    return instance;
}


Rooks::~Rooks()
{
    // nada
}


Rooks::Rooks()
{
    // nada
}


BitBoard Rooks::getAttacksFrom(BitBoard rooks,
                               BitBoard targets,
                               BitBoard friendlies)
{
    BitBoard clear = ~(friendlies | targets);
    
    BitBoard northAttacks = shiftN(smearN(rooks, clear));
    BitBoard southAttacks = shiftS(smearS(rooks, clear));
    BitBoard eastAttacks = shiftE(smearE(rooks, clear));
    BitBoard westAttacks = shiftW(smearW(rooks, clear));
    BitBoard attacks = 
        northAttacks | southAttacks | 
        eastAttacks | westAttacks;
    attacks &= ~friendlies;

    return attacks;
}
