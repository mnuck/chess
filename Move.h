#ifndef __MOVE_H__
#define __MOVE_H__

#include <iostream>

#include "BitBoard.h"
#include "Enums.h"

namespace BixNix
{

class Move
{
public:
    Move(): _data(0x0000) {}
    Move(uint32_t data): _data(data) {}
    Move(Square source, Square target);
    Move(Square source, Square target, bool capturing);
    Move(Square source, Square target, Piece piece);
    Move(Square source, Square target, Piece piece, bool capturing);
    Move(
        Square source,
        Square target,
        Piece movingPiece,
        Piece capturedPiece,
        Piece promotionPiece,
        bool promoting,
        bool capturing,
        bool doublePushing,
        bool enPassanting,
        int enPassantTargetFile,
        bool castling,
        bool castlingDirection);
    operator uint32_t() const { return _data; }

    void setSource(Square s);
    void setTarget(Square t);

    void setMovingPiece(Piece p);
    void setCapturedPiece(Piece p);
    void setPromotionPiece(Piece p);

    void setPromoting(bool flag);
    void setCapturing(bool flag);
    void setDoublePushing(bool flag);
    void setEnPassanting(bool flag);
    void setEnPassantTargetFile(const int file);
    void setCastling(bool flag);
    void setCastlingDirection(bool flag); // short = true

    Square getSource() const;
    Square getTarget() const;

    Piece getMovingPiece() const;
    Piece getCapturedPiece() const;
    Piece getPromotionPiece() const;

    bool getPromoting() const;
    bool getCapturing() const;
    bool getDoublePushing() const;
    bool getEnPassanting() const;
    int  getEnPassantTargetFile() const;
    
    bool getCastling() const;
    bool getCastlingDirection() const;

    bool operator==(const Move& rhs) const;

    int score;

private:
    uint32_t _data;

    // LSB to MSB, bits per item
    // 6 source square
    // 6 target square
    // 2 promotion type
    // 1 if we're promoting
    // 1 if we're capturing
    // 3 piece type
    // 3 target type
    // 1 if this is a double pawn push
    // 1 if this is an en passant capture
    // 3 for en passant target file
    // 1 if we're castling
    // 1 for castling direction
    // 3 unused
    static const uint32_t SOURCE_MASK       = 0b00000000000000000000000000111111;
    static const uint32_t TARGET_MASK       = 0b00000000000000000000111111000000;
    static const uint32_t PROMO_TYPE_MASK   = 0b00000000000000000011000000000000;
    static const uint32_t PROMO_FLAG_MASK   = 0b00000000000000000100000000000000;
    static const uint32_t CAPTURE_FLAG_MASK = 0b00000000000000001000000000000000;
    static const uint32_t PIECE_TYPE_MASK   = 0b00000000000001110000000000000000;
    static const uint32_t TARGET_TYPE_MASK  = 0b00000000001110000000000000000000;
    static const uint32_t DPUSH_FLAG_MASK   = 0b00000000010000000000000000000000;
    static const uint32_t EP_CAP_FLAG_MASK  = 0b00000000100000000000000000000000;
    static const uint32_t EP_FILE_MASK      = 0b00000111000000000000000000000000;
    static const uint32_t CASTL_FLAG_MASK   = 0b00001000000000000000000000000000;
    static const uint32_t CASTL_DIR_MASK    = 0b00010000000000000000000000000000;
};

std::ostream& operator<<(std::ostream& lhs, const Move& rhs);

}


#endif // __MOVE_H_
