#ifndef _ROOKS_H_
#define _ROOKS_H_

#include <vector>
#include <array>

#include "BitBoard.h"

namespace BixNix
{

class Rooks
{
public:
    static Rooks& GetInstance();
    virtual ~Rooks();
    BitBoard getAttacksFrom(BitBoard attackers,
                            BitBoard targets,
                            BitBoard friendlies);

    BitBoard getAttacksFrom(Square square,
                            BitBoard targets,
                            BitBoard friendlies);

protected:
    Rooks();

    std::vector<BitBoard> genOccupancyVariations(Square square);

    std::array<BitBoard, 64> _moveMask;
    std::array<unsigned int, 64> _magicShift;
    std::array<BitBoard, 64> _magicNumber;
    std::array<BitBoard*, 64> _magicAttacks;
    BitBoard* _data;
};

}

#endif // _ROOKS_H_
