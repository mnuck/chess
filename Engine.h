//
// Engine.h
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <chrono>
#include <climits>
#include <condition_variable>
#include <thread>

#include "Enums.h"
#include "Board.h"
#include "TranspositionTable.h"

namespace BixNix
{

class Engine
{
public:
    Engine();
    ~Engine();

    void init(Color color, float time);
    void end();

    void reportMove(Move move, float time);    
    Move getMove();

private:

    int negamax(Board& board,
                const unsigned int depth,
                int alpha=-CHECKMATE,
                int beta=CHECKMATE,
                size_t pvHeight=1);

    int quiescent(Board& board,
                  int alpha,
                  int beta);

    void startSearch();
    void stopSearch();
    void search();

    void trimTrifoldRepetition(const Board& board, std::vector<Move>& moves) const;

    Board _board;
    Color _color;
    float _time;

    static const int HEIGHTMAX = 1000;
    std::array<Move, HEIGHTMAX> _pv;

    std::thread* _searcher;
    bool _search_stop;
    Move _best_move;
    std::condition_variable _best_move_ready;
    std::mutex _cvMutex;

    unsigned long long _node_expansions;
    unsigned long long _cutoffs;
    std::chrono::time_point<std::chrono::system_clock> _start_time;
    
    static const int TTSIZE = 16777216;
    TranspositionTable _ttable;
};

}



#endif // __ENGINE_H__
