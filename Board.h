//
// Board.h
//

#ifndef __BOARD_H__
#define __BOARD_H__

#include <array>
#include <iostream>
#include <functional>

#include "BitBoard.h"
#include "BNStack.h"
#include "Enums.h"
#include "FramedStack.h"
#include "Move.h"

namespace BixNix {

class Board {
 public:
  friend std::ostream& operator<<(std::ostream& lhs, const Board& rhs);
  friend class Evaluate;
  friend ZobristNumber toKey(const Board& board);

  typedef FramedStack<Move, 750, 100> MoveStack;

  Board();
  Board(const Board& that);
  Board& operator=(const Board& that);
  ~Board();

  bool operator==(const Board& rhs) const;
  bool operator!=(const Board& rhs) const;
  void debug() const;

  void getMoves(const Color color, const bool checkCheckmate = true);

  void applyExternalMove(const Move extMove);

  Color getMover() const { return _toMove; }
  ZobristNumber getHash() const { return _hash; }
  TerminalState getTerminalState() const { return _terminalState; }
  uint64_t perft(const int depth);

  static Board initial();
  static Board parseEPD(std::istream& inFile);

  bool isDraw100();
  bool inCheck(const Color color) const;
  bool inCheckmate(const Color color);
  bool WKingMoved() const { return _dirty & (1L << 3); }
  bool BKingMoved() const { return _dirty & (1L << 59); }

  void applyMove(const Move move);
  void unapplyMove(const Move move);

  Move getPastMove(int i);

  mutable MoveStack _ms;

 private:
  BitBoard getUnsafe(Color color) const;
  bool isUnsafe(Square square, Color color) const;

  void getKingMoves(const Color color) const;
  void getQueenMoves(const Color color) const;
  void getBishopMoves(const Color color) const;
  void getKnightMoves(const Color color) const;
  void getRookMoves(const Color color) const;
  void getPawnMoves(const Color color) const;
  void getPawnDoublePushes(const Color color) const;
  void getPawnAttacks(const Color color) const;
  void getPawnEnPassants(const Color color) const;
  void getCastlingMoves(const Color color) const;

  void getMoves(BitBoard movers,
                std::function<BitBoard(BitBoard)> targetGenerator,
                Piece movingPiece, bool doublePushing = false,
                bool enPassanting = false) const;

  static bool parse(const char square, Color& color, Piece& piece);

  bool WKRookMoved() const { return _dirty & (1L << 0); }
  bool WQRookMoved() const { return _dirty & (1L << 7); }
  bool BKRookMoved() const { return _dirty & (1L << 56); }
  bool BQRookMoved() const { return _dirty & (1L << 63); }

  std::array<BitBoard, 6> _pieces;
  std::array<BitBoard, 2> _colors;
  BNStack<Move, HEIGHTMAX> _moves;
  BNStack<int, HEIGHTMAX> _draw100Counter;
  BitBoard _dirty;
  Color _toMove;
  ZobristNumber _hash;
  int _epAvailable;
  TerminalState _terminalState;
};

std::ostream& operator<<(std::ostream& lhs, const Board& rhs);
}

#endif  // __BOARD_H__
