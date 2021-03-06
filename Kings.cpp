#include "BitBoard.h"
#include "Board.h"
#include "Kings.h"

namespace BixNix {

Kings& Kings::GetInstance() {
  static Kings instance;
  return instance;
}

Kings::~Kings() {
  // nada
}

Kings::Kings() {
  for (Square index = 0; index < 64; ++index) {
    _attacks[index] = generateOneStepsFrom(index);
  }
}

BitBoard Kings::generateOneStepsFrom(Square index) {
  BitBoard king = 1LL << index;
  BitBoard westEdge = king & notAFile;
  BitBoard eastEdge = king & notHFile;

  return (king << 8) | (king >> 8) | (westEdge << 9) | (westEdge << 1) |
         (westEdge >> 7) | (eastEdge << 7) | (eastEdge >> 1) | (eastEdge >> 9);
}

BitBoard Kings::getAttacksFrom(Square kingSquare) {
  return _attacks[kingSquare];
}

BitBoard Kings::getAttacksFrom(BitBoard king, BitBoard obstructions) {
  Square kingSquare = __builtin_ffsll(king) - 1;
  return ~obstructions & getAttacksFrom(kingSquare);
}
}
