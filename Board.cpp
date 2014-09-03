#include <algorithm>
#include <vector>

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


Board Board::applyMove(Move move)
{
    Board result(*this);

    const BitBoard source = (1LL << move._source);
    const BitBoard target = (1LL << move._target);

    for (BitBoard& b: result._pieces)
    {
        if (b & source)
        {
            b &= ~source;
            b |= target;
        } 
        else
        {
            if (b & target)
            {
                b &= ~target;
            }
        }
    }

    for (BitBoard& b: result._colors)
    {
        if (b & source)
        {
            b &= ~source;
            b |= target;
        } 
        else 
        {
            if (b & target)
            {
                b &= ~target;
            }
        }
    }

    return result;
}


BitBoard Board::getUnsafe(Color color)
{
    Board::Color otherColor = Board::Color(abs(color - 1));

    BitBoard otherKing = _pieces[Kings] & _colors[otherColor];
    BitBoard otherQueens = _pieces[Queens] & _colors[otherColor];
    BitBoard otherBishops = _pieces[Bishops] & _colors[otherColor];
    BitBoard otherRooks = _pieces[Rooks] & _colors[otherColor];
    BitBoard otherKnights = _pieces[Knights] & _colors[otherColor];
    BitBoard otherPawns = _pieces[Pawns] & _colors[otherColor];
    BitBoard allPieces = _colors[color] & _colors[otherColor];

    return
        Kings::GetInstance().getAttacksFrom(otherKing, 0LL, 0LL) |
        Queens::GetInstance().getAttacksFrom(otherQueens, allPieces, _colors[otherColor]) |
        Bishops::GetInstance().getAttacksFrom(otherBishops, allPieces, _colors[otherColor]) |
        Rooks::GetInstance().getAttacksFrom(otherRooks, allPieces, _colors[otherColor]) |
        Knights::GetInstance().getAttacksFrom(otherKnights, 0LL) |
        Pawns::GetInstance().getAttacksFrom(otherPawns, ~0LL, otherColor);
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
    BitBoard king = _pieces[Kings] & _colors[color];
    return Kings::GetInstance().getAttacksFrom(king, getUnsafe(color), _colors[color]);
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


std::vector<Move> Board::getKingMoves(Color color)
{
    BitBoard movers = _pieces[Kings] & _colors[color];
    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return Kings::GetInstance().getAttacksFrom(mover, 
                                                   getUnsafe(color), 
                                                   _colors[color]);
    };

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getQueenMoves(Color color)
{
    BitBoard movers = _pieces[Queens] & _colors[color];
    Board::Color otherColor = Board::Color(abs(color - 1));

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
    BitBoard movers = _pieces[Bishops] & _colors[color];
    Board::Color otherColor = Board::Color(abs(color - 1));

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
    BitBoard movers = _pieces[Knights] & _colors[color];

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
    return Knights::GetInstance().getAttacksFrom(mover, _colors[color]);    
    };    

    return getMoves(movers, targetGenerator);
}


std::vector<Move> Board::getRookMoves(Color color)
{
    BitBoard movers = _pieces[Rooks] & _colors[color];
    Board::Color otherColor = Board::Color(abs(color - 1));

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
    BitBoard movers = _pieces[Pawns] & _colors[color];
    Board::Color otherColor = Board::Color(abs(color - 1));

    auto targetGenerator = [&] (BitBoard mover) -> BitBoard
    {
        return 
            Pawns::GetInstance().getAttacksFrom(mover, _colors[otherColor], color) |
            Pawns::GetInstance().getMovesFrom(mover, _colors[color] | _colors[otherColor], color);
    };    

    return getMoves(movers, targetGenerator);
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

    return result;
}


Board Board::initial()
{
    Board result;
    result._pieces[Pawns]   = 0x00FF00000000FF00;
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
