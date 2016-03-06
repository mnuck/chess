#ifndef _ZOBRIST_H_
#define _ZOBRIST_H_

#include "Enums.h"

namespace BixNix {

class Zobrist {
 public:
  static Zobrist& GetInstance();
  virtual ~Zobrist() {}

  ZobristNumber getZobrist(Color color, Piece piece, Square square);
  ZobristNumber getEPFile(int file);
  ZobristNumber getBlackToMove() { return _blackToMove; }
  ZobristNumber getWQCastle() { return _WQCastle; }
  ZobristNumber getWKCastle() { return _WKCastle; }
  ZobristNumber getBQCastle() { return _BQCastle; }
  ZobristNumber getBKCastle() { return _BKCastle; }

 protected:
  Zobrist();
  std::array<ZobristNumber, 768> _pieces;
  std::array<ZobristNumber, 8> _epFile;
  ZobristNumber _blackToMove;
  ZobristNumber _WQCastle;
  ZobristNumber _WKCastle;
  ZobristNumber _BQCastle;
  ZobristNumber _BKCastle;
};
}

#endif  // _ZOBRIST_H_
