#include <bitset>

#include "BitBoard.h"

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
