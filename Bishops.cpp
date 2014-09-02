#include "BitBoard.h"
#include "Board.h"
#include "Bishops.h"

Bishops& Bishops::GetInstance()
{
    static Bishops instance;
    return instance;
}


Bishops::~Bishops()
{
    // nada
}


Bishops::Bishops()
{
    // nada
}


BitBoard Bishops::getAttacksFrom(BitBoard bishops,
                                 BitBoard targets,
                                 BitBoard friendlies)
{
    BitBoard clear = ~(friendlies | targets);
    
    BitBoard neAttacks = shiftNE(smearNE(bishops, clear));
    BitBoard seAttacks = shiftSE(smearSE(bishops, clear));
    BitBoard swAttacks = shiftSW(smearSW(bishops, clear));
    BitBoard nwAttacks = shiftNW(smearNW(bishops, clear));
    BitBoard attacks = 
        neAttacks | seAttacks | swAttacks | nwAttacks;
    attacks &= ~friendlies;

    return attacks;
}
