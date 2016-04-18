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

    std::vector<Move> getMoves(const Color color, const bool checkCheckmate=true);

    void applyExternalMove(const Move extMove);

    ZobristNumber getHash() const { return _hash; }
    TerminalState getTerminalState() const { return _terminalState; }
    uint64_t perft(const int depth);

    static Board initial();
    static Board parseEPD(std::istream& inFile);

    bool inCheck(const Color color) const;
    bool inCheckmate(const Color color);
    void applyMove(const Move move);
    void unapplyMove(const Move move);
    
private:

    BitBoard getUnsafe(Color color) const;
    bool isUnsafe(Square square, Color color) const;

    std::vector<Move> getKingMoves(const Color color) const;
    std::vector<Move> getQueenMoves(const Color color) const;
    std::vector<Move> getBishopMoves(const Color color) const;
    std::vector<Move> getKnightMoves(const Color color) const;
    std::vector<Move> getRookMoves(const Color color) const;
    std::vector<Move> getPawnMoves(const Color color) const;
    std::vector<Move> getPawnDoublePushes(const Color color) const;
    std::vector<Move> getPawnAttacks(const Color color) const;
    std::vector<Move> getPawnEnPassants(const Color color) const;
    std::vector<Move> getCastlingMoves(const Color color) const;

    std::vector<Move> getMoves(BitBoard movers, 
                               std::function<BitBoard (BitBoard)> targetGenerator, 
                               Piece movingPiece,
                               bool doublePushing=false,
                               bool enPassanting=false) const;

    static bool parse(const char square, Color& color, Piece& piece);

    bool WKingMoved() const  {return _dirty & (1L <<  3);}
    bool BKingMoved() const  {return _dirty & (1L << 59);}
    bool WKRookMoved() const {return _dirty & (1L <<  0);}
    bool WQRookMoved() const {return _dirty & (1L <<  7);}
    bool BKRookMoved() const {return _dirty & (1L << 56);}
    bool BQRookMoved() const {return _dirty & (1L << 63);}

    std::array<BitBoard, 6> _pieces;
    std::array<BitBoard, 2> _colors;
    std::vector<Move> _moves;
    BitBoard _dirty;
    Color _toMove;
    ZobristNumber _hash;
    int  _epAvailable;
    TerminalState _terminalState;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);

}

#endif // __BOARD_H__


// position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - moves a1b1 e8c8 e5d7
