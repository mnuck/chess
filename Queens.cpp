#include "BitBoard.h"
#include "Board.h"
#include "Queens.h"

Queens& Queens::GetInstance()
{
    static Queens instance;
    return instance;
}


Queens::~Queens()
{
    // nada
}


Queens::Queens()
{
    // nada
}


BitBoard Queens::getAttacksFrom(BitBoard queens,
                                BitBoard targets,
                                BitBoard friendlies)
{
    return 0LL;
}
