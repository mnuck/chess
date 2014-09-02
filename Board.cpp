#include <algorithm>

#include "Board.h"
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
    *this = that;
}


Board& Board::operator=(const Board& that)
{
    _pieces = that._pieces;
    _colors = that._colors;
    return *this;
}


Board::~Board() {}


BitBoard Board::getKingAttacks(Color color)
{
    Board::Color otherColor = Board::Color(abs(color - 1));

    BitBoard king = _pieces[Kings] & _colors[color];
    BitBoard otherKing = _pieces[Kings] & _colors[otherColor];
    BitBoard otherQueens = _pieces[Queens] & _colors[otherColor];
    BitBoard otherBishops = _pieces[Bishops] & _colors[otherColor];
    BitBoard otherRooks = _pieces[Rooks] & _colors[otherColor];
    BitBoard otherKnights = _pieces[Knights] & _colors[otherColor];
    BitBoard otherPawns = _pieces[Pawns] & _colors[otherColor];
    BitBoard allPieces = _colors[color] & _colors[otherColor];

    BitBoard unsafe =
        Kings::GetInstance().getAttacksFrom(otherKing, 0LL, 0LL) |
        Queens::GetInstance().getAttacksFrom(otherQueens, allPieces, _colors[otherColor]) |
        Bishops::GetInstance().getAttacksFrom(otherBishops, allPieces, _colors[otherColor]) |
        Rooks::GetInstance().getAttacksFrom(otherRooks, allPieces, _colors[otherColor]) |
        Knights::GetInstance().getAttacksFrom(otherKnights, 0LL) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, allPieces, otherColor);
    
    return Kings::GetInstance().getAttacksFrom(king, unsafe, _colors[color]);
}


BitBoard Board::getQueenAttacks(Color color)
{
    Board::Color otherColor = Board::Color(abs(color - 1));

    BitBoard queens = _pieces[Queens] & _colors[color];
    return Queens::GetInstance().getAttacksFrom(queens,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getBishopAttacks(Color color)
{
    Board::Color otherColor = Board::Color(abs(color - 1));

    BitBoard bishops = _pieces[Bishops] & _colors[color];
    return Bishops::GetInstance().getAttacksFrom(bishops,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getKnightAttacks(Color color)
{
    BitBoard knights = _pieces[Knights] & _colors[color];
    BitBoard obstructions = _colors[color];
    return Knights::GetInstance().getAttacksFrom(knights, obstructions);    
}


BitBoard Board::getRookAttacks(Color color)
{
    Board::Color otherColor = Board::Color(abs(color - 1));

    BitBoard rooks = _pieces[Rooks] & _colors[color];
    return Bishops::GetInstance().getAttacksFrom(rooks,
                                                _colors[color],
                                                _colors[otherColor]);
}


BitBoard Board::getPawnAttacks(Color color)
{
    BitBoard attackers = _pieces[Pawns] & _colors[color];
    BitBoard targets = _colors[1 - color];
    return Pawns::GetInstance().getAttacksFrom(attackers, targets, color);
}


Board Board::initial()
{
    Board result;
    result._pieces[Pawns]   = 0x00FF00000000FF00;
    result._pieces[Pawns]   = 0x0000000000000000;
    result._pieces[Rooks]   = 0x8100000000000081;
    result._pieces[Knights] = 0x4200000000000042;
    result._pieces[Bishops] = 0x2400000000000024;
    result._pieces[Queens]  = 0x1000000000000010;
    result._pieces[Kings]   = 0x0800000000000008;
    result._colors[Black]   = 0xFFFF000000000000;
    result._colors[White]   = 0x000000000000FFFF;

    return result;
}


std::ostream& operator<<(std::ostream& lhs, const Board& rhs)
{
    auto charAt = [&](size_t i) -> char
    {
        if (rhs._pieces[Board::Pawns] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'p';
        if (rhs._pieces[Board::Pawns] & 
            rhs._colors[Board::Black] &
            (1LL << i)) return 'P';
        if (rhs._pieces[Board::Rooks] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'r';
        if (rhs._pieces[Board::Rooks] & 
            rhs._colors[Board::Black] &
            (1LL << i)) return 'R';
        if (rhs._pieces[Board::Knights] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'n';
        if (rhs._pieces[Board::Knights] & 
            rhs._colors[Board::Black] &
            (1LL << i)) return 'N';
        if (rhs._pieces[Board::Bishops] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'b';
        if (rhs._pieces[Board::Bishops] & 
            rhs._colors[Board::Black] &
            (1LL << i)) return 'B';
        if (rhs._pieces[Board::Queens] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'q';
        if (rhs._pieces[Board::Queens] & 
            rhs._colors[Board::Black] &
            (1LL << i)) return 'Q';
        if (rhs._pieces[Board::Kings] & 
            rhs._colors[Board::White] &
            (1LL << i)) return 'k';
        if (rhs._pieces[Board::Kings] & 
            rhs._colors[Board::Black] &
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
