#include <algorithm>
#include <fstream>
#include <vector>

#include "Board.h"
#include "Enums.h"
#include "Bishops.h"
#include "Kings.h"
#include "Knights.h"
#include "Pawns.h"
#include "Rooks.h"
#include "Zobrist.h"

#include "Logger.h"

namespace BixNix
{

Board::Board() :
    _dirty(0LL),
    _toMove(White),
    _hash(0),
    _epAvailable(-1),
    _terminalState(Running)
{
    _pieces.fill(0LL);
    _colors.fill(0LL);
}


Board::Board(const Board& that):
    _pieces(that._pieces),
    _colors(that._colors),
    _moves(that._moves),
    _dirty(that._dirty),
    _toMove(that._toMove),
    _hash(that._hash),
    _epAvailable(that._epAvailable),
    _terminalState(that._terminalState)
{
    
}


Board& Board::operator=(const Board& that)
{
    _pieces = that._pieces;
    _colors = that._colors;
    _dirty  = that._dirty;
    _moves  = that._moves;
    _toMove = that._toMove;
    _hash = that._hash;
    _epAvailable = that._epAvailable;
    _terminalState = that._terminalState;
    return *this;
}

bool Board::operator==(const Board& rhs) const
{
    bool result = true;

    if (_hash != rhs._hash)
    {
        LOG(trace) << "hash is different";
        result = false;
    }
    if (_moves != rhs._moves)
    {
        LOG(trace) << "moves is different";
        result = false;
    }
    if (_dirty != rhs._dirty)
    {
        LOG(trace) << "dirty is different";
        result = false;
    }
    if (_toMove != rhs._toMove)
    {
        LOG(trace) << "toMove is different";
        result = false;
    }
    if (_epAvailable != rhs._epAvailable)
    {
        LOG(trace) << "EP Avail is different";
        result = false;
    }
//    if (_terminalState != rhs._terminalState)
//    {
//        LOG(trace) << "TS is different";
//        result = false;
//    }
    if (_colors != rhs._colors)
    {
        LOG(trace) << "Colors is different";
        result = false;
    }
    if (_pieces != rhs._pieces)
    {
        LOG(trace) << "Pieces is different";
        result = false;
    }

    if (!result)
    {
        LOG(trace) << "hash\n" << _hash << "\n" << rhs._hash;
        LOG(trace) << "ts " << _terminalState << " " << rhs._terminalState;
        LOG(trace) << "ep " << _epAvailable << " "<< rhs._epAvailable;
        LOG(trace) << "toMove " << _toMove << " " << rhs._toMove;

        
        LOG(trace) << "LHS moves";
        for (auto& m : _moves)
        {
            LOG(trace) << m;
        }

        LOG(trace) << "RHS moves";
        for (auto& m : rhs._moves)
        {
            LOG(trace) << m;
        }

        LOG(trace) << "dirty\n" << RenderBB(_dirty) << "\n---\n" << RenderBB(rhs._dirty);
        LOG(trace) << "colors";
        LOG(trace) << "White\n" << RenderBB(_colors[White]) << "\n---\n" << RenderBB(rhs._colors[White]);
        LOG(trace) << "Black\n" << RenderBB(_colors[Black]) << "\n---\n" << RenderBB(rhs._colors[Black]);
        LOG(trace) << "pieces";
        LOG(trace) << "Knight\n" << RenderBB(_pieces[Knight]) << "\n---\n" << RenderBB(rhs._pieces[Knight]);
        LOG(trace) << "Rook\n" << RenderBB(_pieces[Rook]) << "\n---\n" << RenderBB(rhs._pieces[Rook]);
        LOG(trace) << "Bishop\n" << RenderBB(_pieces[Bishop]) << "\n---\n" << RenderBB(rhs._pieces[Bishop]);
        LOG(trace) << "Queen\n" << RenderBB(_pieces[Queen]) << "\n---\n" << RenderBB(rhs._pieces[Queen]);
        LOG(trace) << "King\n" << RenderBB(_pieces[King]) << "\n---\n" << RenderBB(rhs._pieces[King]);
        LOG(trace) << "Pawn\n" << RenderBB(_pieces[Pawn]) << "\n---\n" << RenderBB(rhs._pieces[Pawn]);        
    }

    return result;
}

bool Board::operator!=(const Board& rhs) const
{
    return !(*this == rhs);
}


void Board::debug()
{
    LOG(trace) << "hash\n" << _hash;
    LOG(trace) << "ts " << _terminalState;
    LOG(trace) << "ep " << _epAvailable;
    LOG(trace) << "toMove " << _toMove;

    LOG(trace) << "moves";
    for (auto& m : _moves)
    {
        LOG(trace) << m;
    }

    LOG(trace) << "dirty\n" << RenderBB(_dirty);
    LOG(trace) << "colors";
    LOG(trace) << "White\n" << RenderBB(_colors[White]);
    LOG(trace) << "Black\n" << RenderBB(_colors[Black]);
    
    LOG(trace) << "pieces";
    LOG(trace) << "Knight\n" << RenderBB(_pieces[Knight]);
    LOG(trace) << "Rook\n" << RenderBB(_pieces[Rook]);
    LOG(trace) << "Bishop\n" << RenderBB(_pieces[Bishop]);
    LOG(trace) << "Queen\n" << RenderBB(_pieces[Queen]);
    LOG(trace) << "King\n" << RenderBB(_pieces[King]);
    LOG(trace) << "Pawn\n" << RenderBB(_pieces[Pawn]);
    
}

Board::~Board() {}

void Board::applyExternalMove(const Move extMove)
{
    // moves that come from the outside don't have all the handy flags
    // so analyze it and set the proper flags
    Square sourceSq(extMove.getSource());
    Square targetSq(extMove.getTarget());
    Piece movingPiece = Pawn;
    Piece capturedPiece = Pawn;
    Piece promotionPiece = Pawn;
    bool capturing = false;
    bool promoting = false;
    bool doublePushing = false;
    bool enPassanting = false;
    int enPassantTargetFile = -1;
    bool castling = false;
    bool castlingDirection = false;

    const BitBoard sourceBB(1LL << sourceSq);
    const BitBoard targetBB(1LL << targetSq);

    bool sourceDirtied = !(_dirty & sourceBB);
    bool targetDirtied = !(_dirty & targetBB);


    for (size_t i = 0; i < 6; ++i)
    {
        if (_pieces[i] & sourceBB)
            movingPiece = Piece(i);
        if (_pieces[i] & targetBB)
        {
            capturedPiece = Piece(i);
            capturing = true;            
        }
    }

    if (movingPiece == Pawn)
    {
        const int diff = sourceSq - targetSq;
        if (abs(diff) == 16)
        {
            doublePushing = true;
            const int file = sourceSq & 7;
            enPassantTargetFile = file;
        }
        else if (targetBB & 0xFF000000000000FF)
        {
            promoting = true;
            promotionPiece = extMove.getPromotionPiece();
        }
        else if (((diff & 1) == 1) && !capturing)
        {
            enPassanting = true;
            capturing = true;
            capturedPiece = Pawn;
        }
    }

    if (movingPiece == King)
    {
        const int diff = sourceSq - targetSq;
        if (2 == diff)
        {
            castling = true;
            castlingDirection = true;
        }
        else if (-2 == diff)
        {
            castling = true;
            castlingDirection = false;
        }
    }

    applyMove(
        Move(sourceSq, targetSq,
             movingPiece, capturedPiece, promotionPiece,
             promoting, capturing, doublePushing, enPassanting,
             enPassantTargetFile, castling, castlingDirection,
             sourceDirtied, targetDirtied));
}


void Board::applyMove(const Move move)
{
    if (_terminalState != Running)
        return;

    _moves.push_back(move);
    
    _hash ^= Zobrist::GetInstance().getBlackToMove();
    if (_epAvailable != -1)
    {
        _hash ^= Zobrist::GetInstance().getEPFile(_epAvailable);
        _epAvailable = -1;
    }
    
    const Square sourceSq(move.getSource());
    const Square targetSq(move.getTarget());
    const BitBoard source(1LL << sourceSq);
    const BitBoard target(1LL << targetSq);
    const Piece movingPiece(move.getMovingPiece());

    Color movingColor = _toMove;
    Color targetColor = Color(1 - _toMove);
    _toMove = targetColor;

    _pieces[movingPiece] &= ~source;
    _pieces[movingPiece] |= target;
    _colors[movingColor] &= ~source;
    _colors[movingColor] |= target;
    _dirty |= source;
    _dirty |= target;
    _hash ^= Zobrist::GetInstance().getZobrist(movingColor, movingPiece, sourceSq);
    _hash ^= Zobrist::GetInstance().getZobrist(movingColor, movingPiece, targetSq);

    if (move.getEnPassanting())
    {
        BitBoard realTargetBB;
        Square   realTargetSq;
        if (White == movingColor)
        {
            realTargetBB = target >> 8;
            realTargetSq = targetSq - 8;
        } 
        else 
        {
            realTargetBB = target << 8;
            realTargetSq = targetSq + 8;
        }
        _colors[targetColor] &= ~realTargetBB;
        _pieces[Pawn] &= ~realTargetBB;
        _hash ^= Zobrist::GetInstance().getZobrist(targetColor, Pawn, realTargetSq);
    }
    else if (move.getCapturing())
    {
        const Piece capturedPiece(move.getCapturedPiece());
        if (capturedPiece != movingPiece)
            _pieces[capturedPiece] &= ~target;
        _colors[targetColor] &= ~target;
        _hash ^= Zobrist::GetInstance().getZobrist(targetColor, capturedPiece, targetSq);
    }

    if (move.getPromoting())
    {
        const Piece promotionPiece(move.getPromotionPiece());
        _pieces[Pawn] &= ~target;
        _pieces[promotionPiece] |= target;
        _hash ^= Zobrist::GetInstance().getZobrist(movingColor, Pawn, targetSq);
        _hash ^= Zobrist::GetInstance().getZobrist(movingColor, promotionPiece, targetSq);
    }

    if (move.getDoublePushing())
    {
        int file(move.getEnPassantTargetFile());
        _epAvailable = file;
        _hash ^= Zobrist::GetInstance().getEPFile(file);        
    }

    if (move.getCastling())
    {
        Square rookSource;
        Square rookTarget;
        if (White == movingColor)
        {
            _hash ^= Zobrist::GetInstance().getWKCastle();
            if (move.getCastlingDirection() == true)
            {
                _hash ^= Zobrist::GetInstance().getWKCastle();
                rookSource = 0;
                rookTarget = 2;
            } else {
                _hash ^= Zobrist::GetInstance().getWQCastle();
                rookSource = 7;
                rookTarget = 4;
            }
        } else {
            if (move.getCastlingDirection() == true)
            {
                _hash ^= Zobrist::GetInstance().getBKCastle();
                rookSource = 56;
                rookTarget = 58;
            } else {
                _hash ^= Zobrist::GetInstance().getBQCastle();
                rookSource = 63;
                rookTarget = 60;
            }
        }
        const BitBoard rookSourceBB(1LL << rookSource);
        const BitBoard rookTargetBB(1LL << rookTarget);
        _pieces[Rook]        &= ~rookSourceBB;
        _pieces[Rook]        |=  rookTargetBB;
        _colors[movingColor] &= ~rookSourceBB;
        _colors[movingColor] |=  rookTargetBB;
        _dirty |= rookSourceBB;
        _hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookSource);
        _hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookTarget);
    }
}


void Board::unapplyMove(const Move move)
{
    _terminalState = Running;

    _moves.pop_back();

    _hash ^= Zobrist::GetInstance().getBlackToMove();
    if (_epAvailable != -1)
    {
        _hash ^= Zobrist::GetInstance().getEPFile(_epAvailable);
        _epAvailable = -1;
    }

    const Square sourceSq(move.getSource());
    const Square targetSq(move.getTarget());
    const BitBoard source(1LL << sourceSq);
    const BitBoard target(1LL << targetSq);
    const Piece movingPiece(move.getMovingPiece());

    Color movingColor = Color(1 - _toMove);
    Color targetColor = _toMove;
    _toMove = movingColor;

    _pieces[movingPiece] |= source;
    _pieces[movingPiece] &= ~target;
    _colors[_toMove] |= source;
    _colors[_toMove] &= ~target;
    if (move.getSourceDirtied())
        _dirty &= ~source;
    if (move.getTargetDirtied())
        _dirty &= ~target;
    _hash ^= Zobrist::GetInstance().getZobrist(movingColor, movingPiece, targetSq);
    _hash ^= Zobrist::GetInstance().getZobrist(movingColor, movingPiece, sourceSq);

    if (move.getEnPassanting())
    {
        BitBoard realTargetBB;
        Square   realTargetSq;
        if (White == movingColor)
        {
            realTargetBB = target >> 8;
            realTargetSq = targetSq - 8;
        } 
        else 
        {
            realTargetBB = target << 8;
            realTargetSq = targetSq + 8;
        }
        _colors[targetColor] |= realTargetBB;
        _pieces[Pawn] |= realTargetBB;
        _hash ^= Zobrist::GetInstance().getZobrist(targetColor, Pawn, realTargetSq);
    }
    else if (move.getCapturing())
    {
        const Piece capturedPiece(move.getCapturedPiece());
        _pieces[capturedPiece] |= target;
        _colors[targetColor] |= target;
        _hash ^= Zobrist::GetInstance().getZobrist(targetColor, capturedPiece, targetSq);
    }

    if (move.getPromoting())
    {
        const Piece promotionPiece(move.getPromotionPiece());
        _pieces[Pawn] |= source;
        _pieces[promotionPiece] &= ~source;
        _hash ^= Zobrist::GetInstance().getZobrist(movingColor, Pawn, targetSq);
        _hash ^= Zobrist::GetInstance().getZobrist(movingColor, promotionPiece, targetSq);
    }

    size_t movesSize = _moves.size();
    if (movesSize > 0)
    {
        const Move& previousMove(_moves[movesSize - 1]);
        if (previousMove.getDoublePushing())
        {
            int file(previousMove.getEnPassantTargetFile());
            _epAvailable = file;
            _hash ^= Zobrist::GetInstance().getEPFile(file);
        }
    }

    if (move.getCastling())
    {
        Square rookSource;
        Square rookTarget;
        if (White == _toMove)
        {
            _hash ^= Zobrist::GetInstance().getWKCastle();
            if (move.getCastlingDirection() == true)
            {
                _hash ^= Zobrist::GetInstance().getWKCastle();
                rookSource = 0;
                rookTarget = 2;
            } else {
                _hash ^= Zobrist::GetInstance().getWQCastle();
                rookSource = 7;
                rookTarget = 4;
            }
        } else {
            if (move.getCastlingDirection() == true)
            {
                _hash ^= Zobrist::GetInstance().getBKCastle();
                rookSource = 56;
                rookTarget = 58;
            } else {
                _hash ^= Zobrist::GetInstance().getBQCastle();
                rookSource = 63;
                rookTarget = 60;
            }
        }
        const BitBoard rookSourceBB(1LL << rookSource);
        const BitBoard rookTargetBB(1LL << rookTarget);
        _pieces[Rook]    |= rookSourceBB;
        _pieces[Rook]    &= ~rookTargetBB;
        _colors[_toMove] |= rookSourceBB;
        _colors[_toMove] &= ~rookTargetBB;
        _dirty &= ~rookSourceBB;
        _hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookSource);
        _hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookTarget);
    }
}


BitBoard Board::getUnsafe(const Color color) const
{
    const Color otherColor = Color(1 - color);

    const BitBoard otherKing = _pieces[King] & _colors[otherColor];
    const BitBoard otherBishopsQueens = (_pieces[Bishop] | _pieces[Queen]) & _colors[otherColor];
    const BitBoard otherRooksQueens = (_pieces[Rook] | _pieces[Queen]) & _colors[otherColor];
    const BitBoard otherKnights = _pieces[Knight] & _colors[otherColor];
    const BitBoard otherPawns = _pieces[Pawn] & _colors[otherColor];
    const BitBoard allPieces = _colors[White] | _colors[Black];

    return
        Kings::GetInstance().getAttacksFrom(otherKing, 0LL) |
        Bishops::GetInstance().getAttacksFrom(otherBishopsQueens, allPieces, 0LL) |
        Rooks::GetInstance().getAttacksFrom(otherRooksQueens, allPieces, 0LL) |
        Knights::GetInstance().getAttacksFrom(otherKnights, 0LL) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, 0xFFFFFFFFFFFFFFFF, otherColor);    
}


bool Board::isUnsafe(Square square, Color color) const
{
    const Color otherColor = Color(1 - color);
    const BitBoard target = (1LL << square);

    const BitBoard allPieces = _colors[White] | _colors[Black];

    const BitBoard otherKing = _pieces[King] & _colors[otherColor];
    if (otherKing & Kings::GetInstance().getAttacksFrom(square))
        return true;

    const BitBoard otherKnights = _pieces[Knight] & _colors[otherColor];
    if (otherKnights & Knights::GetInstance().getAttacksFrom(square))
        return true;

    const BitBoard otherPawns = _pieces[Pawn] & _colors[otherColor];
    if (otherPawns & Pawns::GetInstance().getAttacksFrom(target, 0xFFFFFFFFFFFFFFFFLL, color))
        return true;

    const BitBoard otherBishopsQueens = (_pieces[Bishop] | _pieces[Queen]) & _colors[otherColor];
    if (otherBishopsQueens & Bishops::GetInstance().getAttacksFrom(target, allPieces, 0LL))
        return true;

    const BitBoard otherRooksQueens = (_pieces[Rook] | _pieces[Queen]) & _colors[otherColor];
    if (otherRooksQueens & Rooks::GetInstance().getAttacksFrom(target, allPieces, 0LL))
        return true;
    
    return false;
}


std::vector<Move> Board::getMoves(
    BitBoard movers, 
    std::function<BitBoard (BitBoard)> targetGenerator,
    Piece movingPiece,
    bool doublePushing,
    bool enPassanting) const
{
    std::vector<Move> result;
    result.reserve(400);
    while (0LL != movers)
    {
        const Square source(__builtin_ffsll(movers) - 1);
        BitBoard sourceBoard(1LL << source);
        movers &= ~(1LL << source);
        BitBoard targets(targetGenerator(sourceBoard));
        while (0LL != targets)
        {
            Square target(__builtin_ffsll(targets) - 1);

            targets &= ~(1LL << target);

            BitBoard targetBoard(1LL << target);

            bool dirtyingSource = !(sourceBoard & _dirty);
            bool dirtyingTarget = !(targetBoard & _dirty);

            Piece capturedPiece(Pawn); // covers en passant
            bool capturing((targetBoard & (_colors[White] | _colors[Black])) != 0LL);
            if (enPassanting)
                capturing = true;
            else if (capturing)
            {
                if (targetBoard & _pieces[Knight])
                    capturedPiece = Knight;
                else if (targetBoard & _pieces[Pawn])
                    capturedPiece = Pawn;
                else if (targetBoard & _pieces[Bishop])
                    capturedPiece = Bishop;
                else if (targetBoard & _pieces[Rook])
                    capturedPiece = Rook;
                else if (targetBoard & _pieces[Queen])
                    capturedPiece = Queen;
                else if (targetBoard & _pieces[King])
                    capturedPiece = King;
            }

            if (Pawn == movingPiece)
            {
                if (targetBoard & 0xFF000000000000FFLL)
                {
                    result.emplace_back(Move(source, target, 
                                             Pawn, capturedPiece, Queen, 
                                             true, capturing,
                                             false, false, -1,
                                             false, false,
                                             dirtyingSource,
                                             dirtyingTarget));
                    result.emplace_back(Move(source, target, 
                                             Pawn, capturedPiece, Rook, 
                                             true, capturing,
                                             false, false, -1,
                                             false, false,
                                             dirtyingSource,
                                             dirtyingTarget));
                    result.emplace_back(Move(source, target, 
                                             Pawn, capturedPiece, Bishop, 
                                             true, capturing,
                                             false, false, -1,
                                             false, false,
                                             dirtyingSource,
                                             dirtyingTarget));
                    result.emplace_back(Move(source, target, 
                                             Pawn, capturedPiece, Knight, 
                                             true, capturing,
                                             false, false, -1,
                                             false, false,
                                             dirtyingSource,
                                             dirtyingTarget));
                } 
                else 
                {
                    int enPassantFile = -1;
                    if (doublePushing)
                    {
                        enPassantFile = source % 8;
                    }
                    result.emplace_back(Move(source, target, 
                                             Pawn, capturedPiece, Pawn, 
                                             false, capturing,
                                             doublePushing, enPassanting, enPassantFile,
                                             false, false,
                                             dirtyingSource,
                                             dirtyingTarget));
                }

            }
            else 
            {
                result.emplace_back(Move(source, target, 
                                         movingPiece, capturedPiece, Pawn, 
                                         false, capturing,
                                         false, false, -1,
                                         false, false,
                                         dirtyingSource,
                                         dirtyingTarget));

            }
        }
    }

    return result;
}


std::vector<Move> Board::getKingMoves(const Color color) const
{
    BitBoard movers = _pieces[King] & _colors[color];
    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Kings::GetInstance().getAttacksFrom(mover, _colors[color]);
    };

    return getMoves(movers, targetGenerator, King);
}


std::vector<Move> Board::getQueenMoves(const Color color) const
{
    BitBoard movers = _pieces[Queen] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Rooks::GetInstance().getAttacksFrom(mover,
                                                _colors[otherColor],
                                                _colors[color])
            |
            Bishops::GetInstance().getAttacksFrom(mover,
                                                  _colors[otherColor],
                                                  _colors[color]);
    };

    return getMoves(movers, targetGenerator, Queen);
}


std::vector<Move> Board::getBishopMoves(const Color color) const
{
    BitBoard movers = _pieces[Bishop] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Bishops::GetInstance().getAttacksFrom(mover,
                                                     _colors[otherColor],
                                                     _colors[color]);;
    };

    return getMoves(movers, targetGenerator, Bishop);
}


std::vector<Move> Board::getKnightMoves(const Color color) const
{
    BitBoard movers = _pieces[Knight] & _colors[color];

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Knights::GetInstance().getAttacksFrom(mover, _colors[color]);    
    };    

    return getMoves(movers, targetGenerator, Knight);
}


std::vector<Move> Board::getRookMoves(const Color color) const
{
    BitBoard movers = _pieces[Rook] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Rooks::GetInstance().getAttacksFrom(mover,
                                                   _colors[otherColor],
                                                   _colors[color]);;
    };    

    return getMoves(movers, targetGenerator, Rook);
}


std::vector<Move> Board::getPawnMoves(const Color color) const
{
    BitBoard movers = _pieces[Pawn] & _colors[color];
    Color otherColor = Color(1 - color);
    BitBoard targets = _colors[otherColor];

    if (_epAvailable > -1)
    {
        Square epSquare(40 + _epAvailable);
        if (Black == color)
            epSquare -= 24;

        targets |= (1LL << epSquare);
    }

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        BitBoard blockers(_colors[color] | _colors[otherColor]);
        return 
            Pawns::GetInstance().getMovesFrom(mover, blockers, color);
    };    

    return getMoves(movers, targetGenerator, Pawn);
}


std::vector<Move> Board::getPawnAttacks(const Color color) const
{
    BitBoard movers = _pieces[Pawn] & _colors[color];
    Color otherColor = Color(1 - color);
    BitBoard targets = _colors[otherColor];

    if (_epAvailable > -1)
    {
        Square epSquare(40 + _epAvailable);
        if (Black == color)
            epSquare -= 24;

        targets |= (1LL << epSquare);
    }

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, targets, color);
    };    

    return getMoves(movers, targetGenerator, Pawn);
}


std::vector<Move> Board::getPawnEnPassants(const Color color) const
{
    std::vector<Move> empty;
    if (-1 == _epAvailable)
        return empty;
    
    BitBoard movers(_pieces[Pawn] & _colors[color]);
    Square epSquare(40 + _epAvailable);
    if (White == color)
    {
        movers &= 0x00000000FF000000;
    } 
    else
    {
        movers &= 0x000000FF00000000;
        epSquare -= 24;
    }
    
    BitBoard target(1LL << epSquare);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, target, color);
    };    

    return getMoves(movers, targetGenerator, Pawn, false, true);
}


std::vector<Move> Board::getPawnDoublePushes(const Color color) const
{
    BitBoard movers = _pieces[Pawn] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        BitBoard blockers(_colors[color] | _colors[otherColor]);
        return 
            Pawns::GetInstance().getDoublePushesFrom(mover, blockers, color);
    };    

    return getMoves(movers, targetGenerator, Pawn, true, false);
}


std::vector<Move> Board::getCastlingMoves(const Color color) const
{
    std::vector<Move> result;

    if (White == color)
    {
        if (WKingMoved() || (WKRookMoved() && WQRookMoved()))
            return result; // nobody to castle with

        Square kingLoc(3);
        if (isUnsafe(kingLoc, White))
            return result;

        const BitBoard unsafe(getUnsafe(color));
        const BitBoard blocked(_colors[Black] | _colors[White]);
        const BitBoard noGo(blocked | unsafe);
        if (!WKRookMoved())
        {
            const BitBoard path(6LL);
            if ((path & noGo) == 0LL)
            {
                result.emplace_back(Move(kingLoc, kingLoc - 2,
                                         King, Pawn, Pawn,
                                         false, false, false, false,
                                         -1, true, true, true, true));
            }
        }
        if (!WQRookMoved())
        {
            const BitBoard rookPath(112LL);
            const BitBoard kingPath(48LL);
            if ((kingPath & noGo) == 0LL &&
                (rookPath & blocked) == 0LL)
            {
                result.emplace_back(Move(kingLoc, kingLoc + 2,
                                         King, Pawn, Pawn,
                                         false, false, false, false,
                                         -1, true, false, true, true));
            }
        }
    } 
    else // Black == color
    {
        if (BKingMoved() || (BKRookMoved() && BQRookMoved()))
            return result; // nobody  to castle with

        Square kingLoc(59);
        if (isUnsafe(kingLoc, Black))
            return result;

        const BitBoard unsafe(getUnsafe(color));
        const BitBoard blocked(_colors[Black] | _colors[White]);
        const BitBoard noGo(blocked | unsafe);
        if (!BKRookMoved())
        {
            const BitBoard path(6LL << 56);
            if ((path & noGo) == 0LL)
            {
                result.emplace_back(Move(kingLoc, kingLoc - 2,
                                         King, Pawn, Pawn,
                                         false, false, false, false,
                                         -1, true, true, true, true));
            }
        }
        if (!BQRookMoved())
        {
            const BitBoard rookPath(112LL << 56);
            const BitBoard kingPath(48LL << 56);
            if ((kingPath & noGo) == 0LL &&
                (rookPath & blocked) == 0LL)
            {
                result.emplace_back(Move(kingLoc, kingLoc + 2,
                                         King, Pawn, Pawn,
                                         false, false, false, false,
                                         -1, true, false, true, true));
            }
        }
    }
    
    return result;
}


bool Board::inCheck(const Color color) const
{
    BitBoard kingBoard(_pieces[King] & _colors[color]);
    Square kingSquare(__builtin_ffsll(kingBoard) - 1);
    return isUnsafe(kingSquare, color);
}


bool Board::inCheckmate(const Color color)
{
    if (_terminalState == BlackWin && color == White)
        return true;
    if (_terminalState == WhiteWin && color == Black)
        return true;

    BitBoard kingBoard(_pieces[King] & _colors[color]);

    if (0LL == kingBoard)
    {
        _terminalState = (White == color) ? BlackWin : WhiteWin;
        return true;
    }

    if (inCheck(color))
    {
        auto moves = getMoves(color, false);
        if (moves.size() == 0)
        {
            _terminalState = (White == color) ? BlackWin : WhiteWin;
            return true;            
        }
    }
    return false;
}

std::vector<Move> Board::getMoves(const Color color, const bool checkCheckmate)
{
    std::vector<Move> result;
    if (_terminalState != Running)
    {
        return result;
    }

    if (checkCheckmate && inCheckmate(color))
    {
        _terminalState = (White == color) ? BlackWin : WhiteWin;        
        return result;
    }

    result.reserve(100);
    std::vector<Move> castles(getCastlingMoves(color));
    std::vector<Move> kings(getKingMoves(color));
    std::vector<Move> queens(getQueenMoves(color));
    std::vector<Move> bishops(getBishopMoves(color));
    std::vector<Move> knights(getKnightMoves(color));
    std::vector<Move> rooks(getRookMoves(color));
    std::vector<Move> pawnsMove(getPawnMoves(color));
    std::vector<Move> pawnsDP(getPawnDoublePushes(color));
    std::vector<Move> pawnsAttk(getPawnAttacks(color));
    std::vector<Move> pawnsEP(getPawnEnPassants(color));
    

    result.insert(result.end(), castles.begin(), castles.end());
    result.insert(result.end(), queens.begin(), queens.end());
    result.insert(result.end(), rooks.begin(), rooks.end());
    result.insert(result.end(), bishops.begin(), bishops.end());
    result.insert(result.end(), knights.begin(), knights.end());
    result.insert(result.end(), kings.begin(), kings.end());
    result.insert(result.end(), pawnsMove.begin(), pawnsMove.end());
    result.insert(result.end(), pawnsDP.begin(), pawnsDP.end());
    result.insert(result.end(), pawnsAttk.begin(), pawnsAttk.end());
    result.insert(result.end(), pawnsEP.begin(), pawnsEP.end());

    result.erase(
        std::remove_if(
            result.begin(), 
            result.end(), 
            [this, color] (Move& move) -> bool
            {
                applyMove(move);
                bool bad = inCheck(color);
                unapplyMove(move);
                return bad;
            }),
        result.end());

    if (result.size() == 0)
        _terminalState = Draw;

    return result;
}


Board Board::initial()
{
    Board result;
    result._pieces[Pawn]   = 0x00FF00000000FF00;
    result._pieces[Rook]   = 0x8100000000000081;
    result._pieces[Knight] = 0x4200000000000042;
    result._pieces[Bishop] = 0x2400000000000024;
    result._pieces[Queen]  = 0x1000000000000010;
    result._pieces[King]   = 0x0800000000000008;
    result._colors[Black]  = 0xFFFF000000000000;
    result._colors[White]  = 0x000000000000FFFF;
    result._toMove = White;

    result._hash = 0LL;
    
    return result;
}


uint64_t Board::perft(const int depth)
{
    uint64_t result = 0;

    if (0 == depth)
        return 1;
    
    auto moves(getMoves(_toMove));
    for (const Move& m : moves)
    {
        applyMove(m);
        result += perft(depth - 1);
        unapplyMove(m);
    }

    return result;
}


Board Board::parseEPD(std::istream& in)
{
    Board result;
    std::string piecePlacement;
    std::string sideToMove;
    std::string castling;
    std::string enPassant;

    in >> piecePlacement;
    in >> sideToMove;
    in >> castling;
    in >> enPassant;

    // piece placement
    int index = 63;
    for (char& c : piecePlacement)
    {
        Color color;
        Piece piece;
        if (parse(c, color, piece))
        {
            result._pieces[piece] |= (1LL << index);
            result._colors[color] |= (1LL << index);
            --index;
        }
        else if ('/' == c)
        {
            // this character does nothing
        }
        else // better be a number
        {
            int spaces = c - 48;
            index -= spaces;
        }
    }
    
    // side to move
    if ("w" == sideToMove)
        result._toMove = White;
    else
        result._toMove = Black;
    
    // castling
    BitBoard clean = 0LL;
    if (std::string::npos != castling.find('q'))
        clean |= 0x8800000000000000;
    if (std::string::npos != castling.find('k'))
        clean |= 0x0900000000000000;
    if (std::string::npos != castling.find('Q'))
        clean |= 0x0000000000000088;
    if (std::string::npos != castling.find('K'))
        clean |= 0x0000000000000009;
    result._dirty = ~clean;

    // en passant
    if (std::string::npos != castling.find('a'))
        result._epAvailable = 7;
    if (std::string::npos != castling.find('b'))
        result._epAvailable = 6;
    if (std::string::npos != castling.find('c'))
        result._epAvailable = 5;
    if (std::string::npos != castling.find('d'))
        result._epAvailable = 4;
    if (std::string::npos != castling.find('e'))
        result._epAvailable = 3;
    if (std::string::npos != castling.find('f'))
        result._epAvailable = 2;
    if (std::string::npos != castling.find('g'))
        result._epAvailable = 1;
    if (std::string::npos != castling.find('h'))
        result._epAvailable = 0;

    return result;
}


bool Board::parse(const char square, Color& color, Piece& piece)
{
    switch (square)
    {
    case 'P':
        piece = Pawn;
        color = White;
        return true;        
    case 'p':
        piece = Pawn;
        color = Black;
        return true;        
    case 'R':
        piece = Rook;
        color = White;
        return true;        
    case 'r':
        piece = Rook;
        color = Black;
        return true;        
    case 'N':
        piece = Knight;
        color = White;
        return true;        
    case 'n':
        piece = Knight;
        color = Black;
        return true;        
    case 'B':
        piece = Bishop;
        color = White;
        return true;        
    case 'b':
        piece = Bishop;
        color = Black;
        return true;        
    case 'Q':
        piece = Queen;
        color = White;
        return true;        
    case 'q':
        piece = Queen;
        color = Black;
        return true;        
    case 'K':
        piece = King;
        color = White;
        return true;        
    case 'k':
        piece = King;
        color = Black;
        return true;        
    default:
        return false;        
    }
}


std::ostream& operator<<(std::ostream& lhs, const Board& rhs)
{
    auto charAt = [&](size_t i) -> char
    {
        if (rhs._pieces[Knight] & 
            rhs._colors[White] &
            (1LL << i)) return 'N';
        if (rhs._pieces[Knight] & 
            rhs._colors[Black] &
            (1LL << i)) return 'n';
        if (rhs._pieces[Pawn] & 
            rhs._colors[White] &
            (1LL << i)) return 'P';
        if (rhs._pieces[Pawn] & 
            rhs._colors[Black] &
            (1LL << i)) return 'p';
        if (rhs._pieces[Rook] & 
            rhs._colors[White] &
            (1LL << i)) return 'R';
        if (rhs._pieces[Rook] & 
            rhs._colors[Black] &
            (1LL << i)) return 'r';
        if (rhs._pieces[Bishop] & 
            rhs._colors[White] &
            (1LL << i)) return 'B';
        if (rhs._pieces[Bishop] & 
            rhs._colors[Black] &
            (1LL << i)) return 'b';
        if (rhs._pieces[Queen] & 
            rhs._colors[White] &
            (1LL << i)) return 'Q';
        if (rhs._pieces[Queen] & 
            rhs._colors[Black] &
            (1LL << i)) return 'q';
        if (rhs._pieces[King] & 
            rhs._colors[White] &
            (1LL << i)) return 'K';
        if (rhs._pieces[King] & 
            rhs._colors[Black] &
            (1LL << i)) return 'k';
        return '.';
    };
                        
    for (int i = 63 ; i > -1 ; --i)
    {
        lhs << charAt(i) << " ";
        
        if ((i) % 8 == 0)
        {
            lhs << "\n";
        }
    }

    return lhs;
}

/* white              black
 * 0000 0000 = 0x00   1111 1111 = 0xFF
 * 0000 0000 = 0x00   1111 1111 = 0xFF
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 1111 1111 = 0xFF   0000 0000 = 0x00
 * 1111 1111 = 0xFF   0000 0000 = 0x00

 * pawns              rooks
 * 0000 0000 = 0x00   1000 0001 = 0x81
 * 1111 1111 = 0xFF   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 1111 1111 = 0xFF   0000 0000 = 0x00
 * 0000 0000 = 0x00   1000 0001 = 0x81

 * knights            bishops            
 * 0100 0010 = 0x42   0010 0100 = 0x24
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0100 0010 = 0x42   0010 0100 = 0x24

 * queens             kings
 * 0001 0000 = 0x10   0000 1000 = 0x08
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0000 0000 = 0x00   0000 0000 = 0x00
 * 0001 0000 = 0x10   0000 1000 = 0x08

 */

}
