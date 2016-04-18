#ifndef __ENUMS_H__
#define __ENUMS_H__

#include <cstdint>

namespace BixNix {

typedef uint64_t BitBoard;
typedef uint64_t ZobristNumber;
typedef uint8_t Square;
typedef int16_t Score;
typedef int8_t Depth;

const Score CHECKMATE = 15000;
const Score DRAW = -14999;
const Depth HEIGHTMAX = 64;

enum Piece { Knight, Rook, Bishop, Queen, King, Pawn };

enum Color { White, Black };

enum TerminalState { Running, Draw, WhiteWin, BlackWin };
}

#endif  // __ENUMS_H__
