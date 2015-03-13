#include <iostream>

#include "Move.h"

namespace BixNix
{

Move::Move(Square source, Square target):
    _data(0)
{
    setSource(source);
    setTarget(target);
}

Move::Move(Square source, Square target, Piece piece):
    _data(0)
{
    setSource(source);
    setTarget(target);
    setPromotionPiece(piece);
    setPromoting(true);
}

Move::Move(Square source, Square target, bool capturing):
    _data(0)
{
    setSource(source);
    setTarget(target);
    setCapturing(capturing);
}

Move::Move(Square source, Square target, Piece piece, bool capturing):
    _data(0)
{
    setSource(source);
    setTarget(target);
    setPromotionPiece(piece);
    setPromoting(true);
    setCapturing(capturing);
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
        bool   castlingDirection):
    _data(0)
{
    setSource(source);
    setTarget(target);
    setMovingPiece(movingPiece);
    setCapturedPiece(capturedPiece);
    setPromotionPiece(promotionPiece);
    setPromoting(promoting);
    setCapturing(capturing);
    setDoublePushing(doublePushing);
    setEnPassanting(enPassanting);
    setEnPassantTargetFile(enPassantTargetFile);
    setCastling(castling);
    setCastlingDirection(castlingDirection);
}


void Move::setSource(Square s) 
{ 
    _data &= ~SOURCE_MASK;
    _data |= SOURCE_MASK & uint32_t(s);
}

void Move::setTarget(Square t)
{ 
    _data &= ~TARGET_MASK;
    _data |= (TARGET_MASK & (uint32_t(t) << 6));
}

void Move::setMovingPiece(Piece p)
{ 
    _data &= ~PIECE_TYPE_MASK;
    _data |= (PIECE_TYPE_MASK & (uint32_t(p) << 16));
}

void Move::setCapturedPiece(Piece p)
{ 
    _data &= ~TARGET_TYPE_MASK;
    _data |= (TARGET_TYPE_MASK & (uint32_t(p) << 19));
}

void Move::setPromotionPiece(Piece p)
{ 
    _data &= ~PROMO_TYPE_MASK;
    _data |= (PROMO_TYPE_MASK & (uint32_t(p) << 12));
}

void Move::setEnPassantTargetFile(const int file)
{ 
    _data &= ~EP_FILE_MASK;
    _data |= (EP_FILE_MASK & (uint32_t(file) << 24));
}

void Move::setPromoting(bool flag)
{
    if (flag)
        _data |= PROMO_FLAG_MASK;
    else
        _data &= ~PROMO_FLAG_MASK;
}

void Move::setCapturing(bool flag)
{
    if (flag)
        _data |= CAPTURE_FLAG_MASK;
    else
        _data &= ~CAPTURE_FLAG_MASK;
}

void Move::setDoublePushing(bool flag)
{
    if (flag)
        _data |= DPUSH_FLAG_MASK;
    else
        _data &= ~DPUSH_FLAG_MASK;
}

void Move::setEnPassanting(bool flag)
{
    if (flag)
        _data |= EP_CAP_FLAG_MASK;
    else
        _data &= ~EP_CAP_FLAG_MASK;
}

void Move::setCastling(bool flag)
{
    if (flag)
        _data |= CASTL_FLAG_MASK;
    else
        _data &= ~CASTL_FLAG_MASK;
}

void Move::setCastlingDirection(bool flag) // short = true
{
    if (flag)
        _data |= CASTL_DIR_MASK;
    else
        _data &= ~CASTL_DIR_MASK;
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

