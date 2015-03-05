//
// Engine.h
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <chrono>
#include <condition_variable>
#include <thread>

#include "Enums.h"
#include "Board.h"
#include "TranspositionNode.h"

namespace BixNix
{

class Engine
{
public:
    Engine();
    ~Engine();

    void init(Color color);
    void end();

    void reportTimeLeft(float time);
    void reportMove(Move move);
    
    Move getMove();

private:
    float heuristic(Board& board);
    float minimax(Board& board,
                  const Color toMove,
                  const unsigned int depth,
                  const MinimaxPlayer player=Max,
                  float alpha=-100000,
                  float beta=100000);

    void ponder();

    void trimTrifoldRepetition(Board& board, std::vector<Move>& moves);

    Board _board;
    Color _color;
    float _time;

    std::thread* _ponderer;
    bool _ponderer_done;
    bool _ponderer_needs_new_board;
    Move _ponderer_best_move;
    std::condition_variable _cv_best_move_ready;
    std::mutex _cvMutex;

    unsigned long long _heuristic_runs;
    std::chrono::time_point<std::chrono::system_clock> _start_time;
    unsigned long long _cache_hits;
    unsigned long long _cache_misses;
    
    static const int TTABLE_SIZE = 16777216;
    TranspositionNode* _tTable;
};

}



#endif // __ENGINE_H__
