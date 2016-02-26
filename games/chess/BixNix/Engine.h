//
// Engine.h
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <atomic>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <memory>
#include <thread>

#include "Enums.h"
#include "Board.h"
#include "TranspositionTable.h"
#include "ThreefoldTable.h"

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
    int PVS(Board& board,
            const unsigned int depth,
            int alpha=-CHECKMATE,
            int beta=CHECKMATE,
            size_t pvHeight=1);

    int negamax(const unsigned int depth,
                int alpha=-CHECKMATE,
                int beta=CHECKMATE,
                size_t pvHeight=1);

    int quiescent(Board& board,
                  int alpha,
                  int beta);

    void startSearch();
    void stopSearch();
    void search();

    Board _board;
    Color _color;
    float _time;

    static const int HEIGHTMAX = 16;
    std::array<Move, HEIGHTMAX> _pv; // Principal Variation

    std::shared_ptr<std::thread> _searcher;
    std::atomic_bool _search_stop;
    std::atomic_bool _search_running;

    Move _best_move;
    std::condition_variable _best_move_ready;
    std::condition_variable _searcher_starting;
    

    std::mutex _cvMutex;
    std::mutex _awaitSearcherMutex;
    

    unsigned long long _node_expansions;
    unsigned long long _cutoffs;
    std::chrono::time_point<std::chrono::system_clock> _start_time;

    static const int TTSIZE = 33554432;
    TranspositionTable _ttable;
    ThreefoldTable _3table;
};

}


#endif // __ENGINE_H__
