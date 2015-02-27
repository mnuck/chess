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

namespace BixNix
{

Board::Board()
{
    _pieces.fill(0LL);
    _colors.fill(0LL);
}


Board::Board(const Board& that)
{
    _pieces = that._pieces;
    _colors = that._colors;    
}


Board& Board::operator=(const Board& that)
{
    _pieces = that._pieces;
    _colors = that._colors;
    return *this;
}


Board::~Board() {}


Board Board::applyMove(Move move)
{
    Board result = *this;

    const BitBoard source = (1LL << move.getSource());
    const BitBoard target = (1LL << move.getTarget());

    for (size_t i = 0; i < 6; ++i)
    {
        if (result._pieces[i] & source)
        {
            result._pieces[i] &= ~source;
            result._pieces[i] |= target;
            continue;
        }
        if (result._pieces[i] & target)
        {
            result._pieces[i] &= ~target;
            continue;
        }
    }

    for (size_t i = 0; i < 2; ++i)
    {
        if (result._colors[i] & source)
        {
            result._colors[i] &= ~source;
            result._colors[i] |= target;
            continue;
        } 
        if (result._colors[i] & target)
        {
            result._colors[i] &= ~target;
            continue;
        }
    }

    // promotions
    if (result._pieces[Pawn] & target & 0xFF000000000000FFLL)
    {
        result._pieces[Pawn] &= ~target;
        result._pieces[move.getPiece()] |= target;
    }

    // castling
    if (result._pieces[King] & target)
    {
        if (2 == move.getSource() - move.getTarget())
        {
            if (result._colors[White] & target)
            {
                result._pieces[Rook] &=  0xFFFFFFFFFFFFFFFELL;
                result._colors[White] &= 0xFFFFFFFFFFFFFFFELL;
                result._pieces[Rook] |=  0x0000000000000004LL;
                result._colors[White] |= 0x0000000000000004LL;
            } 
            else 
            {
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
                result._pieces[Rook] &=  0xFFFFFFFFFFFFFF7FLL;
                result._colors[White] &= 0xFFFFFFFFFFFFFF7FLL;
                result._pieces[Rook] |=  0x0000000000000010LL;
                result._colors[White] |= 0x0000000000000010LL;
            }
            else
            {
                result._pieces[Rook] &=  0x7FFFFFFFFFFFFFFFLL;
                result._colors[Black] &= 0x7FFFFFFFFFFFFFFFLL;
                result._pieces[Rook] |=  0x1000000000000000LL;
                result._colors[Black] |= 0x1000000000000000LL;
            }
        }
    }

    return result;
}


bool Board::good()
{
    return _pieces[Pawn]   |
           _pieces[Rook]   |
           _pieces[Knight] |
           _pieces[Bishop] |
           _pieces[Queen];    
}


BitBoard Board::getUnsafe(Color color)
{
    Color otherColor = Color(1 - color);

    BitBoard otherKing = _pieces[King] & _colors[otherColor];
    BitBoard otherQueens = _pieces[Queen] & _colors[otherColor];
    BitBoard otherBishops = _pieces[Bishop] & _colors[otherColor];
    BitBoard otherRooks = _pieces[Rook] & _colors[otherColor];
    BitBoard otherKnights = _pieces[Knight] & _colors[otherColor];
    BitBoard otherPawns = _pieces[Pawn] & _colors[otherColor];

    return
        Kings::GetInstance().getAttacksFrom(otherKing, _colors[otherColor]) |
        Queens::GetInstance().getAttacksFrom(otherQueens, _colors[color], _colors[otherColor]) |
        Bishops::GetInstance().getAttacksFrom(otherBishops, _colors[color], _colors[otherColor]) |
        Rooks::GetInstance().getAttacksFrom(otherRooks, _colors[color], _colors[otherColor]) |
        Knights::GetInstance().getAttacksFrom(otherKnights, _colors[otherColor]) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, _colors[color], otherColor);    
}


std::vector<Move> Board::getMoves(BitBoard movers, std::function<BitBoard (BitBoard)> targetGenerator)
{
    std::vector<Move> result;
    while (0LL != movers)
    {
        Square source = __builtin_ffsll(movers) - 1;
        BitBoard sourceBoard = 1LL << source;
        movers &= ~(1LL << source);
        BitBoard targets = targetGenerator(sourceBoard);
        while (0LL != targets)
        {
            Square target = __builtin_ffsll(targets) - 1;
            targets &= ~(1LL << target);
            result.push_back(Move(source, target));
        }
    }

    return result;
}


BitBoard Board::getKingAttacks(Color color)
{    
    BitBoard king = _pieces[King] & _colors[color];
    return Kings::GetInstance().getAttacksFrom(king, _colors[color]);
}


BitBoard Board::getQueenAttacks(Color color)
{
    Color otherColor = Color(1 - color);

    BitBoard queens = _pieces[Queen] & _colors[color];
    return Queens::GetInstance().getAttacksFrom(queens,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getBishopAttacks(Color color)
{
    Color otherColor = Color(1 - color);

    BitBoard bishops = _pieces[Bishop] & _colors[color];
    return Bishops::GetInstance().getAttacksFrom(bishops,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getKnightAttacks(Color color)
{
    BitBoard knights = _pieces[Knight] & _colors[color];
    BitBoard obstructions = _colors[color];
    return Knights::GetInstance().getAttacksFrom(knights, obstructions);    
}


BitBoard Board::getRookAttacks(Color color)
{
    Color otherColor = Color(1 - color);

    BitBoard rooks = _pieces[Rook] & _colors[color];
    return Bishops::GetInstance().getAttacksFrom(rooks,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getPawnAttacks(Color color)
{
    BitBoard attackers = _pieces[Pawn] & _colors[color];
    BitBoard targets = _colors[1 - color];
    return Pawns::GetInstance().getAttacksFrom(attackers, targets, color);
}    


std::vector<Move> Board::getKingMoves(Color color)
{
    BitBoard movers = _pieces[King] & _colors[color];
    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Kings::GetInstance().getAttacksFrom(mover, _colors[color]);
    };

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getQueenMoves(Color color)
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


std::vector<Move> Board::getBishopMoves(Color color)
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


std::vector<Move> Board::getKnightMoves(Color color)
{
    BitBoard movers = _pieces[Knight] & _colors[color];

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
    return Knights::GetInstance().getAttacksFrom(mover, _colors[color]);    
    };    

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getRookMoves(Color color)
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


std::vector<Move> Board::getPawnMoves(Color color)
{
    BitBoard movers = _pieces[Pawn] & _colors[color];
    Color otherColor = Color(1 - color);

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, _colors[otherColor], color) |
            Pawns::GetInstance().getMovesFrom(mover, _colors[color] | _colors[otherColor], color);
    };    

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getCastlingMoves(Color color)
{
    std::vector<Move> result;
    BitBoard kingInitialLoc;
    if (White == color)
        kingInitialLoc = 0x0000000000000008;
    else
        kingInitialLoc = 0x0800000000000000;

    if ((_pieces[King] & _colors[color]) != kingInitialLoc)
        // king is not in initial position
        return result;
    
    Square kingLoc = __builtin_ffsll(kingInitialLoc) - 1;
    for (Move& m: _moves)
    {
        if (m.getSource() == kingLoc)
        {
            // king has moved
            return result;
        }
    }

    BitBoard unsafe = getUnsafe(color);
    if (unsafe & kingInitialLoc)
        // king is in check
        return result;

    BitBoard noGo = _colors[Black] | _colors[White] | unsafe;

    auto checkSide = [&](BitBoard path, 
                         BitBoard rookInitialLoc,
                         int moveOffset) -> void
    {
        if (path & noGo)
            // path is unusable
            return;
    
        if ((_pieces[Rook] & _colors[color] & rookInitialLoc) == 0LL)
            // rook is not in initial position
            return;

        Square rookLoc = __builtin_ffsll(rookInitialLoc) - 1;
        for (Move& m: _moves)
            if (m.getSource() == rookLoc)
                // rook has moved
                return;

        result.push_back(Move(kingLoc, kingLoc + moveOffset));
    };

    checkSide(3LL << (kingLoc - 2), kingInitialLoc >> 3, -2); // kingside
    checkSide(7LL << (kingLoc + 1), kingInitialLoc << 4,  2); // queenside
    
    return result;
}
    

bool Board::inCheck(Color color)
{
    BitBoard unsafe = getUnsafe(color);
    return (unsafe & _pieces[King] & _colors[color]);
}


std::vector<Move> Board::getMoves(Color color)
{
    std::vector<Move> result;
    std::vector<Move> kings = getKingMoves(color);
    std::vector<Move> queens = getQueenMoves(color);
    std::vector<Move> bishops = getBishopMoves(color);
    std::vector<Move> knights = getKnightMoves(color);
    std::vector<Move> rooks = getRookMoves(color);
    std::vector<Move> pawns = getPawnMoves(color);
    std::vector<Move> castles = getCastlingMoves(color);

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
                Board b = this->applyMove(move);
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
    
    return result;    
}

Board Board::parse(char* filename)
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
