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
    int heuristic(const Board& board);

    int negamax(const Board& board,
                  const Color toMove,
                  const unsigned int depth,
                  int alpha=-100000,
                  int beta=100000);

    /*
    int MTDF(const Board& board,
             const Color toMove,
             int f,
             int d);
    */

    void ponder();

    void trimTrifoldRepetition(const Board& board, std::vector<Move>& moves) const;

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
    unsigned long long _cache_collisions;
    
    static const int TTABLE_SIZE = 10485760;
    TranspositionNode* _tTable;
};

}



#endif // __ENGINE_H__
