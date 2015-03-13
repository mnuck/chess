#include <algorithm>
#include <fstream>
#include <vector>

#include "Board.h"
#include "Enums.h"
#include "Bishops.h"
#include "Kings.h"
#include "Knights.h"
#include "Pawns.h"
#include "Queens.h"
#include "Rooks.h"
#include "Zobrist.h"

namespace BixNix
{

Board::Board() :
    _dirty(0LL),
    _toMove(White),
    _hash(0),
    _epAvailable(-1)
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
    _epAvailable(that._epAvailable)
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
    return *this;
}


Board::~Board() {}

Board Board::applyMove(const Move extMove) const
{
    // moves that come from the outside don't have all the handy flags
    // so analyze it and set the proper flags
    Move move;
    Square sourceSq(extMove.getSource());
    Square targetSq(extMove.getTarget());

    move.setSource(sourceSq);
    move.setTarget(targetSq);

    const BitBoard sourceBB(1LL << sourceSq);
    const BitBoard targetBB(1LL << targetSq);

    for (size_t i = 0; i < 6; ++i)
    {
        if (_pieces[i] & sourceBB)
            move.setMovingPiece(Piece(i));
        if (_pieces[i] & targetBB)
        {
            move.setCapturedPiece(Piece(i));
            move.setCapturing(true);
        }
    }

    if (move.getMovingPiece() == Pawn)
    {
        const int diff = sourceSq - targetSq;
        if (abs(diff) == 16)
        {
            move.setDoublePushing(true);
            const int file = sourceSq & 7;
            move.setEnPassantTargetFile(file);
        }
        else if (targetBB & 0xFF000000000000FF)
        {
            move.setPromoting(true);
            move.setPromotionPiece(extMove.getPromotionPiece());
        }
        else if (((diff & 1) == 1) && 
                 !move.getCapturing())
        {
            move.setEnPassanting(true);
            move.setCapturing(true);
            move.setCapturedPiece(Pawn);
        }
    }

    if (move.getMovingPiece() == King)
    {
        const int diff = sourceSq - targetSq;
        if (2 == diff)
        {
            move.setCastling(true);
            move.setCastlingDirection(true);
        }
        else if (-2 == diff)
        {
            move.setCastling(true);
            move.setCastlingDirection(false);
        }
    }

    return applyInternalMove(move);
}


Board Board::applyInternalMove(const Move move) const
{
    Board result(*this);

    std::copy(result._moves.begin() + 1,
              result._moves.end(),
              result._moves.begin());
    result._moves[6] = move;
    
    result._toMove = Color(1 - _toMove);
    result._hash ^= Zobrist::GetInstance().getBlackToMove();
    if (_epAvailable != -1)
    {
        result._hash ^= Zobrist::GetInstance().getEPFile(_epAvailable);
        result._epAvailable = -1;
    }
    
    const Square sourceSq(move.getSource());
    const Square targetSq(move.getTarget());
    const BitBoard source(1LL << sourceSq);
    const BitBoard target(1LL << targetSq);
    const Piece movingPiece(move.getMovingPiece());

    result._pieces[movingPiece] &= ~source;
    result._pieces[movingPiece] |= target;
    result._colors[_toMove] &= ~source;
    result._colors[_toMove] |= target;
    result._dirty |= source;
    result._dirty |= target;
    result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, movingPiece, sourceSq);
    result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, movingPiece, targetSq);

    if (move.getEnPassanting())
    {
        BitBoard realTargetBB;
        Square   realTargetSq;
        if (White == _toMove)
        {
            realTargetBB = target >> 8;
            realTargetSq = targetSq - 8;
            result._colors[Black] &= ~realTargetBB; 
        } 
        else 
        {
            realTargetBB = target << 8;
            realTargetSq = targetSq + 8;
            result._colors[White] &= ~realTargetBB;
        }
        result._pieces[Pawn] &= ~realTargetBB;
        result._hash ^= Zobrist::GetInstance().getZobrist(Color(1 - _toMove), Pawn, realTargetSq);
    }
    else if (move.getCapturing())
    {
        const Piece capturedPiece(move.getCapturedPiece());
        if (capturedPiece != movingPiece)
            result._pieces[capturedPiece] &= ~target;
        result._colors[1 - _toMove] &= ~target;
        result._hash ^= Zobrist::GetInstance().getZobrist(Color(1 - _toMove), capturedPiece, targetSq);
    }

    if (move.getPromoting())
    {
        const Piece promotionPiece(move.getPromotionPiece());
        result._pieces[Pawn] &= ~target;
        result._pieces[promotionPiece] |= target;
        result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, Pawn, targetSq);
        result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, promotionPiece, targetSq);
    }

    if (move.getDoublePushing())
    {
        int file(move.getEnPassantTargetFile());
        result._epAvailable = file;
        result._hash ^= Zobrist::GetInstance().getEPFile(file);        
    }

    if (move.getCastling())
    {
        Square rookSource;
        Square rookTarget;
        if (White == _toMove)
        {
            result._hash ^= Zobrist::GetInstance().getWKCastle();
            if (move.getCastlingDirection() == true)
            {
                result._hash ^= Zobrist::GetInstance().getWKCastle();
                rookSource = 0;
                rookTarget = 2;
            } else {
                result._hash ^= Zobrist::GetInstance().getWQCastle();
                rookSource = 7;
                rookTarget = 4;
            }
        } else {
            if (move.getCastlingDirection() == true)
            {
                result._hash ^= Zobrist::GetInstance().getBKCastle();
                rookSource = 56;
                rookTarget = 58;
            } else {
                result._hash ^= Zobrist::GetInstance().getBQCastle();
                rookSource = 63;
                rookTarget = 60;
            }
        }
        const BitBoard rookSourceBB(1LL << rookSource);
        const BitBoard rookTargetBB(1LL << rookTarget);
        result._pieces[Rook]    &= ~rookSourceBB;
        result._pieces[Rook]    |=  rookTargetBB;
        result._colors[_toMove] &= ~rookSourceBB;
        result._colors[_toMove] |=  rookTargetBB;
        result._dirty |= rookSourceBB;
        result._dirty |= rookTargetBB;
        result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookSource);
        result._hash ^= Zobrist::GetInstance().getZobrist(_toMove, Rook, rookTarget);
    }

    return result;
}


BitBoard Board::getUnsafe(const Color color) const
{
    const Color otherColor = Color(1 - color);

    const BitBoard otherKing = _pieces[King] & _colors[otherColor];
    const BitBoard otherQueens = _pieces[Queen] & _colors[otherColor];
    const BitBoard otherBishops = _pieces[Bishop] & _colors[otherColor];
    const BitBoard otherRooks = _pieces[Rook] & _colors[otherColor];
    const BitBoard otherKnights = _pieces[Knight] & _colors[otherColor];
    const BitBoard otherPawns = _pieces[Pawn] & _colors[otherColor];

    return
        Kings::GetInstance().getAttacksFrom(otherKing, _colors[otherColor]) |
        Queens::GetInstance().getAttacksFrom(otherQueens, _colors[color], _colors[otherColor]) |
        Bishops::GetInstance().getAttacksFrom(otherBishops, _colors[color], _colors[otherColor]) |
        Rooks::GetInstance().getAttacksFrom(otherRooks, _colors[color], _colors[otherColor]) |
        Knights::GetInstance().getAttacksFrom(otherKnights, _colors[otherColor]) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, _colors[color], otherColor);    
}


std::vector<Move> Board::getMoves(
    BitBoard movers, 
    std::function<BitBoard (BitBoard)> targetGenerator) const
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

            bool capturing((targetBoard & (_pieces[White] | _pieces[Black])) != 0LL);

            if (((sourceBoard & _pieces[Pawn]) != 0LL) &&
                ((targetBoard & 0xFF000000000000FF) != 0LL))
            {
                result.emplace_back(Move(source, target, Bishop, capturing));
                result.emplace_back(Move(source, target, Rook, capturing));
                result.emplace_back(Move(source, target, Knight, capturing));
                result.emplace_back(Move(source, target, Queen, capturing));
            }
            else
                result.emplace_back(Move(source, target, capturing));
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

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getQueenMoves(const Color color) const
{
    BitBoard movers = _pieces[Queen] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Queens::GetInstance().getAttacksFrom(mover,
                                                    _colors[otherColor],
                                                    _colors[color]);;
    };

    return getMoves(movers, targetGenerator);
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

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getKnightMoves(const Color color) const
{
    BitBoard movers = _pieces[Knight] & _colors[color];

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
    return Knights::GetInstance().getAttacksFrom(mover, _colors[color]);    
    };    

    return getMoves(movers, targetGenerator);
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

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getPawnMoves(const Color color) const
{
    BitBoard movers = _pieces[Pawn] & _colors[color];
    Color otherColor = Color(1 - color);
    BitBoard targets = _colors[otherColor];

    if (_epAvailable > -1)
    {
        Square epSquare(47 - _epAvailable);
        if (Black == color)
            epSquare -= 24;

        targets |= (1LL << epSquare);
    }

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, targets, color) |
            Pawns::GetInstance().getMovesFrom(mover, _colors[color] | _colors[otherColor], color);
    };    

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getCastlingMoves(const Color color) const
{
    std::vector<Move> result;
    const BitBoard kingInitialLoc((White == color) ? 
                                  0x0000000000000008LL :
                                  0x0800000000000000LL);

    if (White == color)
    {
        if (WKingMoved() || (WKRookMoved() && WQRookMoved()))
            return result; // nobody to castle with

        const BitBoard unsafe(getUnsafe(color));
        if (getUnsafe(color) & kingInitialLoc)
            return result;

        const BitBoard noGo(_colors[Black] | _colors[White] | unsafe);
        Square kingLoc(3);
        if (!WKRookMoved())
        {
            const BitBoard path(8LL);
            if ((path & noGo) == 0LL)
            {
                Move move;
                move.setSource(kingLoc);
                move.setTarget(kingLoc - 2);
                move.setCastling(true);
                move.setCastlingDirection(true);
                result.push_back(move);
            }
        }
        if (!WQRookMoved())
        {
            const BitBoard path(48LL);
            if ((path & noGo) == 0LL)
            {
                Move move;
                move.setSource(kingLoc);
                move.setTarget(kingLoc + 2);
                move.setCastling(true);
                move.setCastlingDirection(false);
                result.push_back(move);
            }
        }
    } 
    else // Black == color
    {
        if (BKingMoved() || (BKRookMoved() && BQRookMoved()))
            return result; // nobody  to castle with

        const BitBoard unsafe(getUnsafe(color));
        if (getUnsafe(color) & kingInitialLoc)
            return result;

        const BitBoard noGo(_colors[Black] | _colors[White] | unsafe);
        Square kingLoc(59);
        if (!BKRookMoved())
        {
            const BitBoard path(8LL << 56);
            if ((path & noGo) == 0LL)
            {
                Move move;
                move.setSource(kingLoc);
                move.setTarget(kingLoc - 2);
                move.setCastling(true);
                move.setCastlingDirection(true);
                result.push_back(move);
            }
        }
        if (!BQRookMoved())
        {
            const BitBoard path(48LL << 56);
            if ((path & noGo) == 0LL)
            {
                Move move;
                move.setSource(kingLoc);
                move.setTarget(kingLoc + 2);
                move.setCastling(true);
                move.setCastlingDirection(false);
                result.push_back(move);
            }
        }
    }
    
    return result;
}


bool Board::inCheck(const Color color) const
{
    return (getUnsafe(color) & _pieces[King] & _colors[color]);
}


bool Board::inCheckmate(const Color color) const
{
    if ((_pieces[King] & _colors[color]) == 0LL)
        return true;

    // FIXME make a version of getUnsafe that only cares about
    // the king's square. if that square is unsafe, then we're
    // unsafe. so maybe early checks and bail as soon as we know.

    if (getUnsafe(color) & _pieces[King] & _colors[color])
    {
        auto moves = getMoves(color, false);
        if (moves.size() == 0)
            return true;
    }
    return false;
}


std::vector<Move> Board::getMoves(const Color color, const bool checkCheckmate) const
{
    std::vector<Move> result;
    result.reserve(100);
    if (checkCheckmate && inCheckmate(color))
        return result;

    std::vector<Move> kings(getKingMoves(color));
    std::vector<Move> queens(getQueenMoves(color));
    std::vector<Move> bishops(getBishopMoves(color));
    std::vector<Move> knights(getKnightMoves(color));
    std::vector<Move> rooks(getRookMoves(color));
    std::vector<Move> pawns(getPawnMoves(color));
    std::vector<Move> castles(getCastlingMoves(color));

    result.insert(result.end(), kings.begin(), kings.end());
    result.insert(result.end(), queens.begin(), queens.end());
    result.insert(result.end(), bishops.begin(), bishops.end());
    result.insert(result.end(), knights.begin(), knights.end());
    result.insert(result.end(), rooks.begin(), rooks.end());
    result.insert(result.end(), pawns.begin(), pawns.end());
    result.insert(result.end(), castles.begin(), castles.end());

    result.erase(
        std::remove_if(
            result.begin(), 
            result.end(), 
            [this, color] (Move& move) -> bool
            {
                Board b = applyMove(move);
                bool bad = b.inCheck(color);
                return bad;
            }),
        result.end());

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
    result._colors[Black]   = 0xFFFF000000000000;
    result._colors[White]   = 0x000000000000FFFF;
    result._toMove = White;

    result._hash = 0LL;
    
    return result;
}


Board Board::parse(std::istream& inFile)
{
    Board result;
    char buffer;
    Color color;
    Piece piece;

    for (int i = 0; i < 64 && inFile.good(); ++i)
    {
        inFile >> buffer;
        if (parse(buffer, color, piece))
        {
            BitBoard mask = (1LL << (63 - i));
            result._pieces[piece] |= mask;
            result._colors[color] |= mask;
        }
    }

    size_t moveCount;
    inFile >> moveCount;
    for (size_t i = 0; i < moveCount; ++i)
    {
        int source;
        int target;
        char promotion;
        inFile >> source;
        inFile >> target;
        inFile >> promotion;
        Move move;
        move.setSource(source);
        move.setTarget(target);
        switch (promotion)
        {
        case 'B':
            move.setPromotionPiece(Bishop);
            break;
        case 'N':
            move.setPromotionPiece(Knight);
            break;
        case 'R':
            move.setPromotionPiece(Rook);
            break;
        case 'Q':
        default:
            move.setPromotionPiece(Queen);
            break;
        }
        std::copy(result._moves.begin() + 1,
                  result._moves.end(), 
                  result._moves.begin());
        result._moves[6] = move;
    }
    
    return result;    
}

Board Board::parse(const char* filename)
{
    Board result;

    std::ifstream inFile(filename);
    result = parse(inFile);
    inFile.close();
    return result;
}


bool Board::parse(const char square, Color& color, Piece& piece)
{
    switch (square)
    {
    case 'p':
        piece = Pawn;
        color = White;
        return true;        
    case 'P':
        piece = Pawn;
        color = Black;
        return true;        
    case 'r':
        piece = Rook;
        color = White;
        return true;        
    case 'R':
        piece = Rook;
        color = Black;
        return true;        
    case 'n':
        piece = Knight;
        color = White;
        return true;        
    case 'N':
        piece = Knight;
        color = Black;
        return true;        
    case 'b':
        piece = Bishop;
        color = White;
        return true;        
    case 'B':
        piece = Bishop;
        color = Black;
        return true;        
    case 'q':
        piece = Queen;
        color = White;
        return true;        
    case 'Q':
        piece = Queen;
        color = Black;
        return true;        
    case 'k':
        piece = King;
        color = White;
        return true;        
    case 'K':
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
        if (rhs._pieces[Pawn] & 
            rhs._colors[White] &
            (1LL << i)) return 'p';
        if (rhs._pieces[Pawn] & 
            rhs._colors[Black] &
            (1LL << i)) return 'P';
        if (rhs._pieces[Rook] & 
            rhs._colors[White] &
            (1LL << i)) return 'r';
        if (rhs._pieces[Rook] & 
            rhs._colors[Black] &
            (1LL << i)) return 'R';
        if (rhs._pieces[Knight] & 
            rhs._colors[White] &
            (1LL << i)) return 'n';
        if (rhs._pieces[Knight] & 
            rhs._colors[Black] &
            (1LL << i)) return 'N';
        if (rhs._pieces[Bishop] & 
            rhs._colors[White] &
            (1LL << i)) return 'b';
        if (rhs._pieces[Bishop] & 
            rhs._colors[Black] &
            (1LL << i)) return 'B';
        if (rhs._pieces[Queen] & 
            rhs._colors[White] &
            (1LL << i)) return 'q';
        if (rhs._pieces[Queen] & 
            rhs._colors[Black] &
            (1LL << i)) return 'Q';
        if (rhs._pieces[King] & 
            rhs._colors[White] &
            (1LL << i)) return 'k';
        if (rhs._pieces[King] & 
            rhs._colors[Black] &
            (1LL << i)) return 'K';
        return '.';
    };
                        
    for (int i = 63 ; i > -1 ; --i)
    {
        lhs << charAt(i);
        
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
