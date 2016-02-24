#include "Logger.h"
#include "Evaluate.h"

namespace BixNix
{

int Evaluate::getEvaluation(Board& board, const Color color)
{
    int result = 0;
    if (board.inCheckmate(board.getMover()))
    {
        if (board.getMover() == color)
            return -CHECKMATE;
        else
            return CHECKMATE;
    }

    result += materialEval(board);
    result += pieceSquareEval(board);
    if (Black == color)
        result *= -1;

    return result;
}


int Evaluate::materialEval(const Board& board)
{
    int result = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        int diff =
            __builtin_popcountll(board._pieces[i] & board._colors[White]) -
            __builtin_popcountll(board._pieces[i] & board._colors[Black]);
        result += diff * _material[i];
    }
    return result;
}


int Evaluate::pieceSquareEval(const Board& board)
{
    int result = 0;
    for (size_t piece = 0; piece < 6; ++piece)
    {
        BitBoard dudes(board._pieces[piece] & board._colors[White]);
        while (0LL != dudes)
        {
            const Square location(__builtin_ffsll(dudes) - 1);
            dudes &= ~(1LL << location);
            result += _pieceSquare[White][piece][63 - location];
        }

        dudes = board._pieces[piece] & board._colors[Black];
        while (0LL != dudes)
        {
            const Square location(__builtin_ffsll(dudes) - 1);
            dudes &= ~(1LL << location);
            result -= _pieceSquare[Black][piece][63 - location];
        }
    }
    return result;
}


Evaluate::Evaluate()
{
    _material[Pawn] = 103;
    _material[Knight] = 325; // 320
    _material[Bishop] = 337; // 330
    _material[Rook] = 511;
    _material[Queen] = 913;
    _material[King] = 20017;

    _pieceSquare[White][Pawn] =
        {0,  0,  0,  0,  0,  0,  0,  0,
         50, 50, 50, 50, 50, 50, 50, 50,
         10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0};

    _pieceSquare[Black][Pawn] =
        {0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10,-20,-20, 10, 10,  5,
         5, -5,-10,  0,  0,-10, -5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5,  5, 10, 25, 25, 10,  5,  5,
         10, 10, 20, 30, 30, 20, 10, 10,
         50, 50, 50, 50, 50, 50, 50, 50,
         0,  0,  0,  0,  0,  0,  0,  0};

    _pieceSquare[White][Knight] =
        {-50,-40,-30,-30,-30,-30,-40,-50,
         -40,-20,  0,  0,  0,  0,-20,-40,
         -30,  0, 10, 15, 15, 10,  0,-30,
         -30,  5, 15, 20, 20, 15,  5,-30,
         -30,  0, 15, 20, 20, 15,  0,-30,
         -30,  5, 10, 15, 15, 10,  5,-30,
         -40,-20,  0,  5,  5,  0,-20,-40,
         -50,-40,-30,-30,-30,-30,-40,-50};

    _pieceSquare[Black][Knight] =
        {-50,-40,-30,-30,-30,-30,-40,-50,
         -40,-20,  0,  5,  5,  0,-20,-40,
         -30,  5, 10, 15, 15, 10,  5,-30,
         -30,  0, 15, 20, 20, 15,  0,-30,
         -30,  5, 15, 20, 20, 15,  5,-30,
         -30,  0, 10, 15, 15, 10,  0,-30,
         -40,-20,  0,  0,  0,  0,-20,-40,
         -50,-40,-30,-30,-30,-30,-40,-50};

    _pieceSquare[White][Bishop] =
        {-20,-10,-10,-10,-10,-10,-10,-20,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -10,  0,  5, 10, 10,  5,  0,-10,
         -10,  5,  5, 10, 10,  5,  5,-10,
         -10,  0, 10, 10, 10, 10,  0,-10,
         -10, 10, 10, 10, 10, 10, 10,-10,
         -10,  5,  0,  0,  0,  0,  5,-10,
         -20,-10,-10,-10,-10,-10,-10,-20};

    _pieceSquare[Black][Bishop] =
        {-20,-10,-10,-10,-10,-10,-10,-20
         -10,  5,  0,  0,  0,  0,  5,-10,
         -10, 10, 10, 10, 10, 10, 10,-10,
         -10,  0, 10, 10, 10, 10,  0,-10,
         -10,  5,  5, 10, 10,  5,  5,-10,
         -10,  0,  5, 10, 10,  5,  0,-10,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -20,-10,-10,-10,-10,-10,-10,-20,};

    _pieceSquare[White][Rook] =
        {0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0};

    _pieceSquare[Black][Rook] =
        {0,  0,  0,  5,  5,  0,  0,  0
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         5, 10, 10, 10, 10, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0};

    _pieceSquare[White][Queen] =
        {-20,-10,-10, -5, -5,-10,-10,-20,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -10,  0,  5,  5,  5,  5,  0,-10,
          -5,  0,  5,  5,  5,  5,  0, -5,
           0,  0,  5,  5,  5,  5,  0, -5,
         -10,  5,  5,  5,  5,  5,  0,-10,
         -10,  0,  5,  0,  0,  0,  0,-10,
         -20,-10,-10, -5, -5,-10,-10,-20};

    _pieceSquare[Black][Queen] =
        {-20,-10,-10, -5, -5,-10,-10,-20,
         -10,  0,  0,  0,  0,  5,  0,-10,
         -10,  0,  5,  5,  5,  5,  5,-10,
          -5,  0,  5,  5,  5,  5,  0, -5,
           0,  0,  5,  5,  5,  5,  0, -5,
         -10,  0,  5,  5,  5,  5,  0,-10,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -20,-10,-10, -5, -5,-10,-10,-20};

    _pieceSquare[White][King] =
        {-30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -20,-30,-30,-40,-40,-30,-30,-20,
         -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20};

    _pieceSquare[Black][King] =
        {20, 30, 10,  0,  0, 10, 30, 20,
         20, 20,  0,  0,  0,  0, 20, 20,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -20,-30,-30,-40,-40,-30,-30,-20,
         -10,-20,-20,-20,-20,-20,-20,-10};
}


Evaluate& Evaluate::GetInstance()
{
    static Evaluate instance;
    return instance;
}

}
