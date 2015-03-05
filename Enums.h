#ifndef __ENUMS_H__
#define __ENUMS_H__

namespace BixNix
{

typedef uint64_t BitBoard;
typedef uint64_t ZobristNumber;

typedef uint8_t Square;


enum Piece
{
    Knight,
    Rook,
    Bishop,
    Queen,
    King,
    Pawn
};

enum Color
{
    White,
    Black
};


enum MinimaxPlayer
{
    Min,
    Max
};


}

#endif // __ENUMS_H__
