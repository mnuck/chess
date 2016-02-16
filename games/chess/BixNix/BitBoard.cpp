#include <bitset>

#include "BitBoard.h"

namespace BixNix
{


std::ostream& operator<<(std::ostream& lhs, const BitBoard& rhs)
{
    lhs << std::bitset<8>(rhs >> 56) << std::endl;
    lhs << std::bitset<8>(rhs >> 48) << std::endl;
    lhs << std::bitset<8>(rhs >> 40) << std::endl;
    lhs << std::bitset<8>(rhs >> 32) << std::endl;
    lhs << std::bitset<8>(rhs >> 24) << std::endl;
    lhs << std::bitset<8>(rhs >> 16) << std::endl;
    lhs << std::bitset<8>(rhs >> 8) << std::endl;
    lhs << std::bitset<8>(rhs >> 0) << std::endl;
    return lhs;
}


BitBoard smearN(BitBoard g, BitBoard p)
{
    g |= p & (g <<  8);
    p &=     (p <<  8);
    g |= p & (g << 16);
    p &=     (p << 16);
    g |= p & (g << 32);
    return g;    
}


BitBoard smearNE(BitBoard g, BitBoard p)
{
    p &= notAFile;
    g |= p & (g <<  7);
    p &=     (p <<  7);
    g |= p & (g << 14);
    p &=     (p << 14);
    g |= p & (g << 28);
    return g;
}


BitBoard smearE(BitBoard g, BitBoard p)
{
    p &= notAFile;
    g |= p & (g >>  1);
    p &=     (p >>  1);
    g |= p & (g >>  2);
    p &=     (p >>  2);
    g |= p & (g >>  4);
    return g;    
}


BitBoard smearSE(BitBoard g, BitBoard p)
{
    p &= notAFile;
    g |= p & (g >>  9);
    p &=     (p >>  9);
    g |= p & (g >> 18);
    p &=     (p >> 18);
    g |= p & (g >> 36);
    return g;
}



BitBoard smearS(BitBoard g, BitBoard p)
{
    g |= p & (g >>  8);
    p &=     (p >>  8);
    g |= p & (g >> 16);
    p &=     (p >> 16);
    g |= p & (g >> 32);
    return g;
}


BitBoard smearSW(BitBoard g, BitBoard p)
{
    p &= notHFile;
    g |= p & (g >>  7);
    p &=     (p >>  7);
    g |= p & (g >> 14);
    p &=     (p >> 14);
    g |= p & (g >> 28);
    return g;
}


BitBoard smearW(BitBoard g, BitBoard p)
{
    p &= notHFile;
    g |= p & (g <<  1);
    p &=     (p <<  1);
    g |= p & (g <<  2);
    p &=     (p <<  2);
    g |= p & (g <<  4);
    return g;
}


BitBoard smearNW(BitBoard g, BitBoard p)
{
    p &= notHFile;
    g |= p & (g <<  9);
    p &=     (p <<  9);
    g |= p & (g << 18);
    p &=     (p << 18);
    g |= p & (g << 36);
    return g;    
}


BitBoard shiftN(BitBoard source) 
{
    return source << 8;
}


BitBoard shiftNE(BitBoard source)
{
    return (source & notHFile) << 7;
}


BitBoard shiftE(BitBoard source)
{
    return (source & notHFile) >> 1;
}


BitBoard shiftSE(BitBoard source)
{
    return (source & notHFile) >> 9;
}


BitBoard shiftS(BitBoard source)
{
    return source >> 8;
}


BitBoard shiftSW(BitBoard source)
{
    return (source & notAFile) >> 7;
}


BitBoard shiftW(BitBoard source)
{
    return (source & notAFile) << 1;
}


BitBoard shiftNW(BitBoard source)
{
    return (source & notAFile) << 9;
}

}
