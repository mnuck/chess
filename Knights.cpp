#include "BitBoard.h"
#include "Knights.h"

namespace BixNix
{

Knights& Knights::GetInstance()
{
    static Knights instance;
    return instance;
}


Knights::~Knights()
{
    // nada
}


BitBoard Knights::getAttacksFrom(Square index)
{
    return _attacks[index];
}


Knights::Knights()
{
    for (Square index = 0 ; index < 64 ; ++index)
    {
        _attacks[index] = generateAttacksFrom(index);
    }
}


BitBoard Knights::generateAttacksFrom(Square index)
{
    BitBoard x = 1LL << index;
    
    return (x & notAFile) << 17  |
           (x & notAFile) >> 15  |
           (x & notABFile) << 6  |
           (x & notABFile) >> 10 |
           (x & notHFile) << 15  |
           (x & notHFile) >> 17  |
           (x & notGHFile) >> 6  |
           (x & notGHFile) << 10;
}


BitBoard Knights::getAttacksFrom(BitBoard attackers, BitBoard obstructions)
{
    BitBoard left1  = (attackers >> 1) & 0x7f7f7f7f7f7f7f7f;
    BitBoard left2  = (attackers >> 2) & 0x3f3f3f3f3f3f3f3f;
    BitBoard right1 = (attackers << 1) & 0xfefefefefefefefe;
    BitBoard right2 = (attackers << 2) & 0xfcfcfcfcfcfcfcfc;
    BitBoard h1 = left1 | right1;
    BitBoard h2 = left2 | right2;
    BitBoard attacks = (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
    return attacks & ~obstructions;
}

}
