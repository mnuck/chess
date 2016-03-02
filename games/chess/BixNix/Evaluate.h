#ifndef _EVALUATE_H_
#define _EVALUATE_H_

#include "Board.h"

namespace BixNix
{

class Evaluate
{
public:
    static Evaluate& GetInstance();
    virtual ~Evaluate() {}
    int getEvaluation(Board& board, const Color color);
    int getEvaluation(const Move& move, const Color color);

protected:
    Evaluate();

    int materialEval(const Board& board);
    int pieceSquareEval(const Board& board);

    std::array<std::array<std::array<int, 64>, 6>, 2> _pieceSquare;
    std::array<int, 6> _material;
};

}

#endif // _EVALUATE_H_
