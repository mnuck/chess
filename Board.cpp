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

Board::Board()
{
    _pieces.fill(0LL);
    _colors.fill(0LL);
    _moves.reserve(50);
}


Board::Board(const Board& that):
    _pieces(that._pieces),
    _colors(that._colors),
    _moves(that._moves),
    _toMove(that._toMove),
    _hash(that._hash)
{
    
}


Board& Board::operator=(const Board& that)
{
    _pieces = that._pieces;
    _colors = that._colors;
    _moves  = that._moves;
    _toMove = that._toMove;
    _hash = that._hash;
    return *this;
}


Board::~Board() {}


Board Board::applyMove(const Move move) const
{
    Board result(*this);

    result._moves.push_back(move);
    result._toMove = Color(1 - _toMove);
    result._hash ^= Zobrist::GetInstance().getBlackToMove();

    const BitBoard source(1LL << move.getSource());
    const BitBoard target(1LL << move.getTarget());

    // en passant check
    if ((source & result._pieces[Pawn]) != 0LL)
    {  // a pawn moved
        const int diff = move.getSource() - move.getTarget();
        if (diff % 2 != 0)
        { // a pawn moved diagonally
            const BitBoard allPieces = result._colors[White] | result._colors[Black];
            if ((target & allPieces) == 0LL)
            {  // a pawn attacked an empty square (en passant occured)
                BitBoard realTarget;
                if ((target & 0x0000FF0000000000) != 0LL)
                {  // white attacker
                    realTarget = target >> 8;
                    result._pieces[Pawn]  &= ~realTarget;
                    result._colors[Black] &= ~realTarget;                    
                } else {
                    realTarget = target << 8;
                    result._pieces[Pawn]  &= ~realTarget;
                    result._colors[White] &= ~realTarget;                    
                }
            }
        }
    }


    Piece movingPiece;
    Piece capturedPiece;
    
    for (size_t i = 0; i < 6; ++i)
    {
        if (result._pieces[i] & source)
        {
            movingPiece = Piece(i);
            capturedPiece = movingPiece;
            result._pieces[i] &= ~source;
            result._pieces[i] |= target;
            continue;
        }
        if (result._pieces[i] & target)
        {
            capturedPiece = Piece(i);
            result._pieces[i] &= ~target;
            continue;
        }
    }


    for (size_t i = 0; i < 2; ++i)
    {
        if (result._colors[i] & source)
        {
            result._hash ^= Zobrist::GetInstance().getZobrist(Color(i), movingPiece, move.getSource());
            result._hash ^= Zobrist::GetInstance().getZobrist(Color(i), movingPiece, move.getTarget());
            result._colors[i] &= ~source;
            result._colors[i] |= target;
            continue;
        } 
        if (result._colors[i] & target)
        {            
            result._hash ^= Zobrist::GetInstance().getZobrist(Color(i), capturedPiece, move.getTarget());
            result._colors[i] &= ~target;
            continue;
        }
    }

    // promotions
    if (result._pieces[Pawn] & target & 0xFF000000000000FFLL)
    {
        result._pieces[Pawn] &= ~target;
        result._pieces[move.getPiece()] |= target;
        if (result._colors[White] & target)
        {
            result._hash ^= Zobrist::GetInstance().getZobrist(White, Pawn, move.getTarget());
            result._hash ^= Zobrist::GetInstance().getZobrist(White, move.getPiece(), move.getTarget());
        } else {
            result._hash ^= Zobrist::GetInstance().getZobrist(Black, Pawn, move.getTarget());
            result._hash ^= Zobrist::GetInstance().getZobrist(Black, move.getPiece(), move.getTarget());
        }
    }

    // castling
    if (result._pieces[King] & target)
    {
        if (2 == move.getSource() - move.getTarget())
        {
            if (result._colors[White] & target)
            {
                result._hash ^= Zobrist::GetInstance().getWKCastle();
                result._hash ^= Zobrist::GetInstance().getZobrist(White, Rook, Square(0));
                result._hash ^= Zobrist::GetInstance().getZobrist(White, Rook, Square(2));                

                result._pieces[Rook] &=  0xFFFFFFFFFFFFFFFELL;
                result._colors[White] &= 0xFFFFFFFFFFFFFFFELL;
                result._pieces[Rook] |=  0x0000000000000004LL;
                result._colors[White] |= 0x0000000000000004LL;
            } 
            else 
            {
                result._hash ^= Zobrist::GetInstance().getBKCastle();
                result._hash ^= Zobrist::GetInstance().getZobrist(Black, Rook, Square(56));
                result._hash ^= Zobrist::GetInstance().getZobrist(Black, Rook, Square(58));                

                result._pieces[Rook] &=  0xFEFFFFFFFFFFFFFFLL;
                result._colors[Black] &= 0xFEFFFFFFFFFFFFFFLL;
                result._pieces[Rook] |=  0x0400000000000000LL;
                result._colors[Black] |= 0x0400000000000000LL;
            }
        }
        else if (2 == move.getTarget() - move.getSource())
        {
            if (result._colors[White] & target)
            {
                result._hash ^= Zobrist::GetInstance().getWQCastle();
                result._hash ^= Zobrist::GetInstance().getZobrist(White, Rook, Square(7));
                result._hash ^= Zobrist::GetInstance().getZobrist(White, Rook, Square(4));

                result._pieces[Rook] &=  0xFFFFFFFFFFFFFF7FLL;
                result._colors[White] &= 0xFFFFFFFFFFFFFF7FLL;
                result._pieces[Rook] |=  0x0000000000000010LL;
                result._colors[White] |= 0x0000000000000010LL;
            }
            else
            {
                result._hash ^= Zobrist::GetInstance().getBKCastle();
                result._hash ^= Zobrist::GetInstance().getZobrist(Black, Rook, Square(63));
                result._hash ^= Zobrist::GetInstance().getZobrist(Black, Rook, Square(60));

                result._pieces[Rook] &=  0x7FFFFFFFFFFFFFFFLL;
                result._colors[Black] &= 0x7FFFFFFFFFFFFFFFLL;
                result._pieces[Rook] |=  0x1000000000000000LL;
                result._colors[Black] |= 0x1000000000000000LL;
            }
        }
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
    result.reserve(21);
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
            result.emplace_back(Move(source, target));

            BitBoard targetBoard(1LL << target);
            if (((sourceBoard & _pieces[Pawn]) != 0LL) &&
                ((targetBoard & 0xFF000000000000FF) != 0LL))
            {
                result.emplace_back(Move(source, target, Bishop));
                result.emplace_back(Move(source, target, Rook));
                result.emplace_back(Move(source, target, Knight));
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

    // en passant handling
    if (_moves.size() > 0)
    {
        BitBoard mover = 1LL << _moves[_moves.size() - 1].getTarget();
        if ((mover & _pieces[Pawn] & _colors[otherColor]) != 0LL)
        {  // a pawn moved
            BitBoard moverSource = 1LL << _moves[_moves.size() - 1].getSource();
            BitBoard pseudoMover = 0LL;
            if (White == color)
            {
                if (((moverSource & 0x00FF000000000000) != 0LL) &&
                    ((mover       & 0x000000FF00000000) != 0LL))
                {  // pawn did a double push
                    pseudoMover = mover << 8;
                }
            } else {
                if (((moverSource & 0x000000000000FF00) != 0LL) &&
                    ((mover       & 0x00000000FF000000) != 0LL))
                {
                    pseudoMover = mover >> 8;
                }
                
            }
            if (pseudoMover != 0LL)
            {
                targets &= ~mover;
                targets |= pseudoMover;
            }            
        }
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

    if ((_pieces[King] & _colors[color]) != kingInitialLoc)
        // king is not in initial position
        return result;
    
    Square kingLoc = __builtin_ffsll(kingInitialLoc) - 1;
    for (const Move& m: _moves)
    {
        if (m.getSource() == kingLoc)
        {
            // king has moved
            return result;
        }
    }

    const BitBoard unsafe(getUnsafe(color));
    if (unsafe & kingInitialLoc)
        // king is in check
        return result;

    BitBoard noGo(_colors[Black] | _colors[White] | unsafe);

    auto checkSide = [&](const BitBoard path, 
                         const BitBoard rookInitialLoc,
                         const int moveOffset) -> void
    {
        if (path & noGo)
            // path is unusable
            return;
    
        if ((_pieces[Rook] & _colors[color] & rookInitialLoc) == 0LL)
            // rook is not in initial position
            return;

        Square rookLoc = __builtin_ffsll(rookInitialLoc) - 1;
        for (const Move& m: _moves)
            if (m.getSource() == rookLoc)
                // rook has moved
                return;

        result.push_back(Move(kingLoc, kingLoc + moveOffset));
    };

    checkSide(3LL << (kingLoc - 2), kingInitialLoc >> 3, -2); // kingside
    checkSide(7LL << (kingLoc + 1), kingInitialLoc << 4,  2); // queenside
    
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
        switch (promotion)
        {
        case 'B':
            result._moves.push_back(Move(source, target, Bishop));
            break;
        case 'N':
            result._moves.push_back(Move(source, target, Knight));
            break;
        case 'R':
            result._moves.push_back(Move(source, target, Rook));
            break;
        case 'Q':
        default:
            result._moves.push_back(Move(source, target, Queen));
            break;
        }
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

    for (const Move& m : rhs._moves)
    {
        lhs << m << "\n";
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
