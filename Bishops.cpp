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
    return 0LL;
}
