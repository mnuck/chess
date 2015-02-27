#include "BitBoard.h"
#include "Board.h"
#include "Queens.h"
#include "Rooks.h"
#include "Bishops.h"

namespace BixNix
{

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


BitBoard Queens::getAttacksFrom(const BitBoard& queens,
                                const BitBoard& targets,
                                const BitBoard& friendlies)
{
    return 
        Bishops::GetInstance().getAttacksFrom(queens, targets, friendlies) |
        Rooks::GetInstance().getAttacksFrom(queens, targets, friendlies);
}

}
