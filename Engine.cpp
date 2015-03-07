#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <vector>

#include "Engine.h"
#include "Evaluate.h"

namespace BixNix
{


void Engine::end() 
{
    auto end_time = std::chrono::system_clock::now();
    auto diff = 
        std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

    std::cout << _heuristic_runs << " heuristic runs" << std::endl
              << diff.count() << " seconds" << std::endl
              << _heuristic_runs / diff.count() << " runs per sec" << std::endl;
    std::cout << _cache_collisions << " cache collisions\n"
              << _cache_hits << " cache hits\n"
              << _cache_misses << " cache misses\n"
              << _cache_hits / static_cast<double>(_cache_hits + _cache_misses) 
              << " ratio" << std::endl;

    unsigned long long occupied = 0;
    for (size_t i = 0; i < TTABLE_SIZE; ++i)
        if (_tTable[i]._hash != 0xFFFFFFFFFFFFFFFFLL)
            ++occupied;
    std::cout << occupied << " cache slots occupied\n"
              << TTABLE_SIZE << " cache slots total\n"
              << occupied / static_cast<double>(TTABLE_SIZE) << " occupancy" << std::endl;

    _ponderer_done = true;
    if (_ponderer != nullptr)
    {
        if (_ponderer->joinable())
        {
            _ponderer->join();
        }
        delete _ponderer;
        _ponderer = nullptr;
    }
}


Move Engine::getMove()
{
    using namespace std::chrono;
    int runTime = std::min((int)_time / 10, 10);
    auto startTime = steady_clock::now();
    auto endTime = startTime + seconds(runTime);

    std::vector<long> splits;

    std::unique_lock<std::mutex> lock(_cvMutex);
    while (std::cv_status::no_timeout == 
            _cv_best_move_ready.wait_until(lock, endTime))
    {
        auto timeSplit = steady_clock::now();
        int diff = duration_cast<microseconds>(timeSplit - startTime).count();
        splits.push_back(diff);

        if (_ponderer_best_move.score == 100000)
        {
            break;
        }
        
        if (splits.size() > 3)
        {
            double ultimate = log(splits[splits.size() - 1]);
            double penultimate = log(splits[splits.size() - 2]);
            double estimate = exp(2 * ultimate - penultimate);

            auto nextMoveEst = startTime + microseconds(static_cast<long>(estimate));
            if (nextMoveEst > endTime)
            {
                break;
            }
        }
    }

    Move move = _ponderer_best_move;
    std::cout << "taking " << move << " ";
    Board pvBoard(_board.applyMove(move));
    TranspositionNode node(_tTable[pvBoard.getHash() % TTABLE_SIZE]);
    int i = 0;
    while (i++ < 10 && node._hash == pvBoard.getHash())
    {
        std::cout << node._move << " ";
        pvBoard = pvBoard.applyMove(node._move);
        node = _tTable[pvBoard.getHash() % TTABLE_SIZE];
    }
    std::cout << std::endl;
    _board = _board.applyMove(move);
    
    return move;
}


void Engine::ponder()
{
    Board ponderBoard;
    while (!_ponderer_done)
    {
        if (_ponderer_needs_new_board)
        {
            ponderBoard = _board;
            _ponderer_needs_new_board = false;
        }
        
        std::vector<Move> potentialMoves(ponderBoard.getMoves(_color));
        trimTrifoldRepetition(ponderBoard, potentialMoves);

        float maxScore;
        unsigned int depth = 0;
        while (!_ponderer_done)
        {
            if (depth > 50)
                std::this_thread::sleep_for(std::chrono::seconds(1));
            
            if (_ponderer_needs_new_board)
                break;
            
            maxScore = -100000;
            for (Move& m: potentialMoves)
            {
                Board b(_board.applyMove(m));
                m.score = -negamax(b, Color(1 - _color), depth);
                if (m.score > maxScore)
                    maxScore = m.score;
                if (_ponderer_done)
                    return;
                if (_ponderer_needs_new_board)
                    break;
            }
            if (_ponderer_needs_new_board)
                break;
            
            std::sort(potentialMoves.begin(), potentialMoves.end(),
                      [&](const Move& a, const Move& b) -> bool
                      { return a.score > b.score; });

            _ponderer_best_move = potentialMoves[0];
            _cv_best_move_ready.notify_all();
            
            for (int i = potentialMoves.size() - 1; i >= 0; --i)
                std::cout << "(" << potentialMoves[i].score << ") " << potentialMoves[i] << std::endl;

            std::cout << "*** " << depth << " " << _ponderer_best_move.score << " " 
                      << _ponderer_best_move << std::endl;


            ++depth;
        }
    }
}


int Engine::negamax(const Board& board,
                    const Color toMove,
                    const unsigned int depth,
                    int alpha,
                    int beta)
{
    if (_ponderer_done || _ponderer_needs_new_board)
        return 0.0;

/*
    TranspositionNode& node = _tTable[board.getHash() % TTABLE_SIZE];
    if (node._hash == board.getHash() &&
        node._depth >= depth)
    {
        ++_cache_hits;
        switch (node._evalType)
        {
        case TranspositionNode::Alpha:
            alpha = std::max(alpha, node._score);
            break;
        case TranspositionNode::Beta:
            beta = std::min(beta, node._score);
            break;
        case TranspositionNode::Actual:
            return node._score;
            break;
        default:
            break;
        }
        if (beta <= alpha)
            return node._score;
    }
    else
        ++_cache_misses;
*/

    if (0 == depth)
    {
        float value = heuristic(board);
        return (toMove == _color) ? value : -value;        
    }
    
    std::vector<Move> actions = board.getMoves(toMove);
    trimTrifoldRepetition(board, actions);
    if (actions.size() == 0)
    {
        float value = heuristic(board);
        return (toMove == _color) ? value : -value;        
    }

    // move ordering
    for (Move& m : actions)
    {
        Board b(board.applyMove(m));
//        const TranspositionNode& bNode = _tTable[b.getHash() % TTABLE_SIZE];
//        if (bNode._hash == b.getHash() && 
//            TranspositionNode:: Actual == bNode._evalType)
//        {
//            m.score =  bNode._score;
//        } else {
            int value = heuristic(b);
            m.score = (toMove == _color) ? value : -value;
//        }
    }

    std::sort(actions.begin(), actions.end(),
              [&](const Move& a, const Move& b) -> bool
              { return a.score > b.score; });

    int result = -100000;
    Move bestMove(actions[0]);
    for (const Move& m: actions)
    {
        Board b(board.applyMove(m));
        int value = -negamax(b, Color(1 - toMove), depth - 1, -beta, -alpha);
        if (_ponderer_done || _ponderer_needs_new_board)
            return 0.0;        

        result = std::max(result, value);
        if (value > alpha)
        {
            alpha = value;
            bestMove = m;
        }
        if (result >= beta)
            break;
    }
    
    {
        TranspositionNode& node = _tTable[board.getHash() % TTABLE_SIZE];
        if (node._hash != 0xFFFFFFFFFFFFFFFFLL && node._hash != board.getHash())
            ++_cache_collisions;

        if (node._hash != board.getHash() ||
            node._depth <= depth)
        {
            node._hash = board._hash;
            node._depth = depth;
            node._score = result;
            node._move = bestMove;
            if (result <= alpha)
                node._evalType = TranspositionNode::Alpha;
            else if (beta <= result)
                node._evalType = TranspositionNode::Beta;
            else
                node._evalType = TranspositionNode::Actual;           
        }
    }    
    
    return result;
}


Engine::Engine():
    _ponderer(nullptr),
    _ponderer_best_move(Move(0,0)),
    _cache_hits(0),
    _cache_misses(0),
    _cache_collisions(0),
    _tTable(nullptr)
{
    _tTable = new TranspositionNode[TTABLE_SIZE];
    for (size_t i = 0; i < TTABLE_SIZE; ++i)
        _tTable[i]._hash = 0xFFFFFFFFFFFFFFFFLL;
}

Engine::~Engine()
{
    if (nullptr == _tTable)
    {
        delete [] _tTable;
        _tTable = nullptr;
    }
}
    
void Engine::init(Color color)
{
    _heuristic_runs = 0;
    _start_time = std::chrono::system_clock::now();

    if (_ponderer == nullptr)
    {
        srand(time(NULL));

        _board = Board::initial();
        _color = color;

        _ponderer_done = false;
        _ponderer_needs_new_board = true;
        _ponderer = new std::thread(&Engine::ponder, this);        
    }
}


int Engine::heuristic(const Board& board)
{
    ++_heuristic_runs;
    return Evaluate::GetInstance().getEvaluation(std::ref(board), _color);
}


void Engine::reportTimeLeft(float time)
{
    _time = time;
}


void Engine::reportMove(Move move)
{
    _board = _board.applyMove(move);
    _ponderer_needs_new_board = true;
}


void Engine::trimTrifoldRepetition(const Board& board, std::vector<Move>& actions) const
{
    // trim trifold repetition moves for now
    actions.erase(
        std::remove_if(
            actions.begin(),
            actions.end(),
            [&] (Move& move) -> bool
            {
                size_t moveLength = board._moves.size();
                if (moveLength >= 7)
                {
                    if ((move == board._moves[moveLength - 4]) &&
                        (board._moves[moveLength - 1] == board._moves[moveLength - 5]) &&
                        (board._moves[moveLength - 2] == board._moves[moveLength - 6]) &&
                        (board._moves[moveLength - 3] == board._moves[moveLength - 7]))
                    {
                        return true;
                    }
                }
                return false;
            }),
        actions.end());    
}


}
