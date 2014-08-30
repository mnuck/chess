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
    return kings & safe;
}
