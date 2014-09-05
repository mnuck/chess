#include <algorithm>
#include <vector>

#include "Board.h"
#include "Enums.h"
#include "Bishops.h"
#include "Kings.h"
#include "Knights.h"
#include "Pawns.h"
#include "Queens.h"
#include "Rooks.h"

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

    const BitBoard source = (1LL << move._source);
    const BitBoard target = (1LL << move._target);

    for (size_t i = 0; i < 6; ++i)
    {
        if (result._pieces[i] & source)
        {
            result._pieces[i] &= ~source;
            result._pieces[i] |= target;
        } 
        else 
        {
            if (result._pieces[i] & target)
            {
                result._pieces[i] &= ~target;
            }
        }
    }

    for (size_t i = 0; i < 2; ++i)
    {
        if (result._colors[i] & source)
        {
            result._colors[i] &= ~source;
            result._colors[i] |= target;
        } 
        else 
        {
            if (result._colors[i] & target)
            {
                result._colors[i] &= ~target;
            }
        }
    }

    if (result._pieces[Pawn] & target & 0xFF000000000000FFLL)
    {
        result._pieces[Pawn] &= ~target;
        result._pieces[move._piece] |= target;
    }

    return result;
}


bool Board::good()
{
    if (_pieces[Pawn])
    {
        return true;
    }
    if (_pieces[Rook])
    {
        return true;
    }
    if (_pieces[Knight])
    {
        return true;
    }
    if (_pieces[Bishop])
    {
        return true;
    }
    if (_pieces[Queen])
    {
        return true;
    }
    return false;
}


BitBoard Board::getUnsafe(Color color)
{
    Color otherColor = Color(abs(color - 1));

    BitBoard otherKing = _pieces[King] & _colors[otherColor];
    BitBoard otherQueens = _pieces[Queen] & _colors[otherColor];
    BitBoard otherBishops = _pieces[Bishop] & _colors[otherColor];
    BitBoard otherRooks = _pieces[Rook] & _colors[otherColor];
    BitBoard otherKnights = _pieces[Knight] & _colors[otherColor];
    BitBoard otherPawns = _pieces[Pawn] & _colors[otherColor];
    BitBoard allPieces = _colors[color] | _colors[otherColor];

    return
        Kings::GetInstance().getAttacksFrom(otherKing, 0LL) |
        Queens::GetInstance().getAttacksFrom(otherQueens, allPieces, _colors[otherColor]) |
        Bishops::GetInstance().getAttacksFrom(otherBishops, allPieces, _colors[otherColor]) |
        Rooks::GetInstance().getAttacksFrom(otherRooks, allPieces, _colors[otherColor]) |
        Knights::GetInstance().getAttacksFrom(otherKnights, 0LL) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, ~0LL, otherColor) |
        0LL;
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
    Color otherColor = Color(abs(color - 1));

    BitBoard queens = _pieces[Queen] & _colors[color];
    return Queens::GetInstance().getAttacksFrom(queens,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getBishopAttacks(Color color)
{
    Color otherColor = Color(abs(color - 1));

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
    Color otherColor = Color(abs(color - 1));

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
    Color otherColor = Color(abs(color - 1));

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
    Color otherColor = Color(abs(color - 1));

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
    Color otherColor = Color(abs(color - 1));

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
    Color otherColor = Color(abs(color - 1));

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, _colors[otherColor], color) |
            Pawns::GetInstance().getMovesFrom(mover, _colors[color] | _colors[otherColor], color);
    };    

    return getMoves(movers, targetGenerator);
}


bool Board::inCheck(Color color)
{
    return (getUnsafe(color) & _pieces[King] & _colors[color]);
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

    result.insert(result.end(), kings.begin(), kings.end());
    result.insert(result.end(), queens.begin(), queens.end());
    result.insert(result.end(), bishops.begin(), bishops.end());
    result.insert(result.end(), knights.begin(), knights.end());
    result.insert(result.end(), rooks.begin(), rooks.end());
    result.insert(result.end(), pawns.begin(), pawns.end());

    result.erase(
        std::remove_if(
            result.begin(), 
            result.end(), 
            [&] (Move& move) -> bool
            {
                return this->applyMove(move).inCheck(color);                
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


#include <fstream>

Board Board::parse(char* filename)
{
    Board result;
    char buffer;
    Color color;
    Piece piece;

    std::ifstream inFile(filename);
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
