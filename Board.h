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

namespace BixNix
{

class Board
{
public:
    friend std::ostream& operator<<(std::ostream& lhs, const Board& rhs);
    friend class Engine;

    Board();
    Board(const Board& that);
    Board& operator=(const Board& that);
    ~Board();

    std::vector<Move> getMoves(const Color color, bool checkCheckmate=true);
    Board applyMove(Move move);
    bool good();

    static Board initial();
    static Board parse(char* filename);
    static Board parse(std::istream& inFile);
    
private:
    BitBoard getUnsafe(Color color) const;

    std::vector<Move> getKingMoves(Color color);
    std::vector<Move> getQueenMoves(Color color);
    std::vector<Move> getBishopMoves(Color color);
    std::vector<Move> getKnightMoves(Color color);
    std::vector<Move> getRookMoves(Color color);
    std::vector<Move> getPawnMoves(Color color);
    std::vector<Move> getCastlingMoves(Color color);

    std::vector<Move> getMoves(BitBoard movers, std::function<BitBoard (BitBoard)> targetGenerator);
    bool inCheck(Color color) const;
    bool inCheckmate(const Color color);

    static bool parse(const char square, Color& color, Piece& piece);

    std::array<BitBoard, 6> _pieces;
    std::array<BitBoard, 2> _colors;
    std::vector<Move> _moves;
    Color _toMove;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

}

#endif // __BOARD_H__
