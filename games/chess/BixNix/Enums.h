#ifndef __ENUMS_H__
#define __ENUMS_H__

#include <cstdint>

namespace BixNix {

const int CHECKMATE = 100000;

typedef uint64_t BitBoard;
typedef uint64_t ZobristNumber;

typedef uint8_t Square;

enum Piece { Knight, Rook, Bishop, Queen, King, Pawn };

enum Color { White, Black };

enum MinimaxPlayer { Min, Max };

enum TerminalState { Running, Draw, WhiteWin, BlackWin };
}

#endif  // __ENUMS_H__
