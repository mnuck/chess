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
    friend class Evaluate;

    Board();
    Board(const Board& that);
    Board& operator=(const Board& that);
    ~Board();

    std::vector<Move> getMoves(const Color color, const bool checkCheckmate=true) const;

    Board applyMove(const Move extMove) const;
    Board applyInternalMove(const Move move) const;

    ZobristNumber getHash() const { return _hash; }

    static Board initial();
    static Board parse(const char* filename);
    static Board parse(std::istream& inFile);
    
private:
    BitBoard getUnsafe(Color color) const;

    std::vector<Move> getKingMoves(const Color color) const;
    std::vector<Move> getQueenMoves(const Color color) const;
    std::vector<Move> getBishopMoves(const Color color) const;
    std::vector<Move> getKnightMoves(const Color color) const;
    std::vector<Move> getRookMoves(const Color color) const;
    std::vector<Move> getPawnMoves(const Color color) const;
    std::vector<Move> getCastlingMoves(const Color color) const;

    std::vector<Move> getMoves(BitBoard movers, std::function<BitBoard (BitBoard)> targetGenerator) const;
    bool inCheck(const Color color) const;
    bool inCheckmate(const Color color) const;

    static bool parse(const char square, Color& color, Piece& piece);

    bool WKingMoved() const  {return _dirty & (1L <<  3);}
    bool BKingMoved() const  {return _dirty & (1L << 59);}
    bool WKRookMoved() const {return _dirty & (1L <<  0);}
    bool WQRookMoved() const {return _dirty & (1L <<  7);}
    bool BKRookMoved() const {return _dirty & (1L << 56);}
    bool BQRookMoved() const {return _dirty & (1L << 63);}

    std::array<BitBoard, 6> _pieces;
    std::array<BitBoard, 2> _colors;
    std::array<Move, 7> _moves;
    BitBoard _dirty;
    Color _toMove;
    ZobristNumber _hash;
    int  _epAvailable;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

}

#endif // __BOARD_H__
