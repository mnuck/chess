//
// Board.h
//

#ifndef __BOARD_H__
#define __BOARD_H__

#include <iostream>
#include <array>

#include "BitBoard.h"

class Board
{
public:
    friend std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

    enum Piece
    {
        Pawns,
        Rooks,
        Knights,
        Bishops,
        Queens,
        Kings
    };

    enum Color
    {
        White,
        Black
    };

    Board();
    Board(const Board& that);
    Board& operator=(const Board& that);
    ~Board();

    BitBoard getKingAttacks(Color color);
    BitBoard getQueenAttacks(Color color);
    BitBoard getBishopAttacks(Color color);
    BitBoard getKnightAttacks(Color color);
    BitBoard getRookAttacks(Color color);
    BitBoard getPawnAttacks(Color color);

    static Board initial();
    
private:
    std::array<BitBoard, 6> _pieces;
    std::array<BitBoard, 2> _colors;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

#endif // __BOARD_H__
