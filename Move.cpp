#include <iostream>

#include "Move.h"

std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
{
    lhs << int(rhs._source) << " -> " << int(rhs._target) << " promote to: " << int(rhs._piece);
    return lhs;
}
