#include <iostream>

#include "Move.h"

namespace BixNix
{

Move::Move(Square source, Square target, Piece piece):
    _data(0)
{
    _data |= SOURCE_MASK & uint32_t(source);
    _data |= (TARGET_MASK & (uint32_t(target) << 6));
    _data |= (PROMO_TYPE_MASK & (uint32_t(piece) << 12));
    _data |= PROMO_FLAG_MASK;
}

Move::Move(
        Square source,
        Square target,
        Piece  movingPiece,
        Piece  capturedPiece,
        Piece  promotionPiece,
        bool   promoting,
        bool   capturing,
        bool   doublePushing,
        bool   enPassanting,
        int    enPassantTargetFile,
        bool   castling,
        bool   castlingDirection,
        bool   sourceDirtied,
        bool   targetDirtied):
    _data(0)
{
    _data |= SOURCE_MASK & uint32_t(source);
    _data |= (TARGET_MASK & (uint32_t(target) << 6));
    _data |= (PIECE_TYPE_MASK & (uint32_t(movingPiece) << 16));
    _data |= (TARGET_TYPE_MASK & (uint32_t(capturedPiece) << 19));
    _data |= (PROMO_TYPE_MASK & (uint32_t(promotionPiece) << 12));
    _data |= (EP_FILE_MASK & (uint32_t(enPassantTargetFile) << 24));

    if (promoting)
        _data |= PROMO_FLAG_MASK;

    if (capturing)
        _data |= CAPTURE_FLAG_MASK;

    if (doublePushing)
        _data |= DPUSH_FLAG_MASK;

    if (enPassanting)
        _data |= EP_CAP_FLAG_MASK;

    if (castling)
        _data |= CASTL_FLAG_MASK;

    if (castlingDirection)
        _data |= CASTL_DIR_MASK;

    if (sourceDirtied)
        _data |= DIRTY_SOURCE_MASK;

    if (targetDirtied)
        _data |= DIRTY_TARGET_MASK;
}

Square Move::getSource() const
{
    return Square(_data & SOURCE_MASK);
}

Square Move::getTarget() const
{
    return Square((_data & TARGET_MASK) >> 6);
}

Piece Move::getPromotionPiece()  const
{
    return Piece((_data & PROMO_TYPE_MASK) >> 12);
}

Piece Move::getMovingPiece()  const
{
    return Piece((_data & PIECE_TYPE_MASK) >> 16);
}

Piece Move::getCapturedPiece()  const
{
    return Piece((_data & TARGET_TYPE_MASK) >> 19);
}

int Move::getEnPassantTargetFile()  const
{
    return Piece((_data & EP_FILE_MASK) >> 24);
}

bool Move::getPromoting() const
{
    return (_data & PROMO_FLAG_MASK);
}

bool Move::getCapturing() const
{
    return (_data & CAPTURE_FLAG_MASK);
}

bool Move::getDoublePushing() const
{
    return (_data & DPUSH_FLAG_MASK);
}

bool Move::getEnPassanting() const
{
    return (_data & EP_CAP_FLAG_MASK);
}

bool Move::getCastling() const
{
    return (_data & CASTL_FLAG_MASK);
}

bool Move::getCastlingDirection() const
{
    return (_data & CASTL_DIR_MASK);
}

bool Move::getSourceDirtied() const
{
    return (_data & DIRTY_SOURCE_MASK);
}

bool Move::getTargetDirtied() const
{
    return (_data & DIRTY_TARGET_MASK);
}


std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
{
    static const char files[] = " abcdefgh";

    const int sourceFile = 8 - (rhs.getSource() % 8);
    const int sourceRank = 1 + (rhs.getSource() / 8);
    const int targetFile = 8 - (rhs.getTarget() % 8);
    const int targetRank = 1 + (rhs.getTarget() / 8);

    lhs << files[sourceFile] << sourceRank
        << files[targetFile] << targetRank;

    if (rhs.getPromoting())
    {
        switch (rhs.getPromotionPiece())
        {
        case Knight:
            lhs << "n";
            break;
        case Rook:
            lhs << "r";
            break;
        case Bishop:
            lhs << "b";
            break;
        case Queen:
            lhs << "q";
            break;
        case King:
            lhs << " promote to a King? You don't vote for kings!";
            break;
        case Pawn:
            lhs << " promote to a...Pawn? What does that even mean?";
            break;
        default:
            lhs << " promote to a...wait, what?";
            break;
        }
    }

    return lhs;
}

bool Move::operator==(const Move& rhs) const
{
    return _data == rhs._data;
}

}
