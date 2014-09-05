//
// Board.h
//

#ifndef __BOARD_H__
#define __BOARD_H__

#include <array>
#include <iostream>
#include <functional>
#include <vector>

#include "BitBoard.h"
#include "Enums.h"
#include "Move.h"

class Board
{
public:
    friend std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

    Board();
    Board(const Board& that);
    Board& operator=(const Board& that);
    ~Board();

    std::vector<Move> getMoves(Color color);
    Board applyMove(Move move);
    bool good();

    static Board initial();
    static Board parse(char* filename);
    
private:
    BitBoard getUnsafe(Color color);
    BitBoard getKingAttacks(Color color);
    BitBoard getQueenAttacks(Color color);
    BitBoard getBishopAttacks(Color color);
    BitBoard getKnightAttacks(Color color);
    BitBoard getRookAttacks(Color color);
    BitBoard getPawnAttacks(Color color);

    std::vector<Move> getKingMoves(Color color);
    std::vector<Move> getQueenMoves(Color color);
    std::vector<Move> getBishopMoves(Color color);
    std::vector<Move> getKnightMoves(Color color);
    std::vector<Move> getRookMoves(Color color);
    std::vector<Move> getPawnMoves(Color color);

    std::vector<Move> getMoves(BitBoard movers, std::function<BitBoard (BitBoard)> targetGenerator);
    bool inCheck(Color color);
    static bool parse(const char square, Color& color, Piece& piece);

    std::array<BitBoard, 6> _pieces;
    std::array<BitBoard, 2> _colors;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

#endif // __BOARD_H__
