#include <algorithm>
#include <chrono>
#include <iomanip>
#include <climits>
#include <ctime>
#include <cmath>
#include <functional>
#include <vector>

#include "Engine.h"
#include "Evaluate.h"

namespace BixNix
{


void Engine::end() 
{
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

    std::cout << _time << " time left" << std::endl;

    auto end_time = std::chrono::system_clock::now();
    auto diff = 
        std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

    std::cout << diff.count() << " seconds "
              << _node_expansions / static_cast<double>(diff.count()) << std::endl;

    std::cout << _node_expansions << " node expansions\n"
              << _cutoffs << " cutoffs\n"
              << _cutoffs / static_cast<double>(_node_expansions) << " cutoff ratio" << std::endl;

    std::cout << _cache_collisions << " cache collisions\n"
              << _cache_hits << " cache hits\n"
              << _cache_misses << " cache misses\n"
              << _cache_hits / static_cast<double>(_cache_hits + _cache_misses) 
              << " ratio" << std::endl;

    unsigned long long occupied = 0;

    for (size_t i = 0; i < TTABLE_SIZE; ++i)
        if (_TTable[i]._hash != 0xFFFFFFFFFFFFFFFFLL)
            ++occupied;
    std::cout << occupied << " cache slots occupied\n"
              << TTABLE_SIZE << " cache slots total\n"
              << occupied / static_cast<double>(TTABLE_SIZE) << " occupancy" << std::endl;
}


Move Engine::getMove()
{
    using namespace std::chrono;
    int runTime = std::min((int)_time / 10, 20);
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

        if (_ponderer_best_move.score >= CHECKMATE)
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
    _board = _board.applyMove(move);
    _ponderer_needs_new_board = true; 
    std::cout << "sending (" << move.score << ") " << move << " " << std::endl;
    std::cout << _board << std::endl;
   
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

        std::vector<Move> actions(ponderBoard.getMoves(ponderBoard._toMove));
        trimTrifoldRepetition(ponderBoard, actions);

        unsigned int depth = 0;
        while (!_ponderer_done)
        {
            if (depth > 50)
                std::this_thread::sleep_for(std::chrono::seconds(1));
            

            for (Move& m : actions)
            {
                Board brd(ponderBoard.applyMove(m));
                m.score = MTDF(brd, m.score, depth);                
                if (_ponderer_needs_new_board)
                    break;
            }
            if (_ponderer_needs_new_board)
                break;

            std::sort(actions.begin(), actions.end(),
                      [&](const Move& a, const Move& b) -> bool
                      { return a.score > b.score; });


//            for (int i = actions.size() - 1; i >= 0; --i)
//                std::cout << "(" << actions[i].score << ") " << actions[i] << std::endl;

            std::cout << "*** " << depth << " (" << actions[0].score << ") " 
                      << actions[0] << std::endl;
/*
            _pv[0] = actions[0];
            std::cout << "PV: ";
            for (Move& m : _pv)
            {
                if (Move(0, 0) == m)
                    break;
                else
                    std::cout << m << " ";
                m = Move(0, 0);
            }
            std::cout << std::endl;            
*/
            _ponderer_best_move = actions[0];
            _cv_best_move_ready.notify_all();

            ++depth;
        }
    }
}


int Engine::MTDF(const Board& board,
                 int guess,
                 const unsigned int depth)
{
    int upper(INT_MAX);
    int lower(INT_MIN);
    while (lower < upper)
    {
        int beta = (guess == lower) ? guess + 1 : guess;
        guess = minimax(std::ref(board), Min, depth, beta - 1, beta);
        if (guess < beta)
            upper = guess;
        else
            lower = guess;
    }
    return guess;
}


int Engine::minimax(const Board& board,
                    const MinimaxPlayer player,
                    const unsigned int depth,
                    int alpha,
                    int beta,
                    const int pvHeight)
{
    ++_node_expansions;
    if (_ponderer_done || _ponderer_needs_new_board)
        return 0;

    int result((Max == player) ? INT_MIN : INT_MAX);
    auto mm = std::bind(
        &Engine::minimax, this, 
        std::placeholders::_1, 
        MinimaxPlayer(1 - player), depth - 1,
        std::placeholders::_2, std::placeholders::_3,
        pvHeight + 1);

    MTDFTTNode& node = _TTable[board.getHash() % TTABLE_SIZE];
    if (node._hash == board.getHash() && node._depth >= depth)
    {
        ++_cache_hits;
        if (node._lower >= beta)
        {
            ++_cutoffs;
            return node._lower;            
        }
        if (node._upper <= alpha)
        {
            ++_cutoffs;
            return node._upper;
        }
        alpha = std::max(alpha, node._lower);
        beta = std::min(beta, node._upper);
    }
    else
        ++_cache_misses;

    if (0 == depth)
    {
        result = heuristic(board);
    }
    else 
    {
        std::vector<Move> actions(board.getMoves(board._toMove));
        trimTrifoldRepetition(board, actions);
        if (actions.size() == 0)
        {
            //result = 0; // stalemate
        }
        else
        {
/*            // move ordering here
            for (Move& m : actions)
            {
                Board brd(board.applyMove(m));
                const MTDFTTNode& bNode = _TTable[brd.getHash() % TTABLE_SIZE];
                if (bNode._hash == brd.getHash() && node._depth >= depth)
                {
                    if (bNode._upper == INT_MAX)
                        m.score = bNode._lower;
                    if (bNode._lower == INT_MIN)
                        m.score = bNode._upper;
                }
                else
                {
                    m.score = heuristic(brd);
                }
                if (m.getCapturing())
                {
                    m.score += (Max == player) ? 10000 : -10000;
                }
            }
*/
            if (Max == player)
            {
//                std::sort(actions.begin(), actions.end(),
//                          [&](const Move& a, const Move& b) -> bool
//                          { return a.score > b.score; });
                // PV nodes first
/*                std::stable_sort(actions.begin(), actions.end(),
                                 [&](const Move& a, const Move& b) -> bool
                                 {
                                     auto itA = std::find(_pv.begin(),_pv.end(), a);
                                     auto itB = std::find(_pv.begin(),_pv.end(), b);
                                     bool foundA = (itA != _pv.end());
                                     bool foundB = (itB != _pv.end());
                                     return foundA && !foundB;
                                 });
*/
                int a = alpha;
                for (Move& m: actions)
                {
                    if (result >= beta)
                    {
                        ++_cutoffs;
                        break;
                    }

                    Board brd(board.applyMove(m));
                    m.score = mm(brd, a, beta);
                    if (_ponderer_done || _ponderer_needs_new_board)
                        return 0;

                    result = std::max(result, m.score);
                    if (result > a)
                    {
                        a = result;
                        _pv[pvHeight] = m;
                    }
                }
            }
            else // Min == player
            {
//                std::sort(actions.begin(), actions.end(),
//                          [&](const Move& a, const Move& b) -> bool
//                          { return a.score < b.score; });
                // PV nodes first
/*                std::stable_sort(actions.begin(), actions.end(),
                                 [&](const Move& a, const Move& b) -> bool
                                 {
                                     auto itA = std::find(_pv.begin(),_pv.end(), a);
                                     auto itB = std::find(_pv.begin(),_pv.end(), b);
                                     bool foundA = (itA != _pv.end());
                                     bool foundB = (itB != _pv.end());
                                     return foundA && !foundB;
                                 });
*/
                int b = beta;
                for (Move& m: actions)
                {
                    if (result <= alpha)
                    {
                        ++_cutoffs;
                        break;
                    }

                    Board brd(board.applyMove(m));
                    m.score = mm(brd, alpha, b);                    
                    if (_ponderer_done || _ponderer_needs_new_board)
                        return 0;

                    result = std::min(result, m.score);
                    if (result < b)
                    {
                        b = result;
                        _pv[pvHeight] = m;
                    }
                    b = std::min(result, b);
                }
            }
        }
    }
    
    MTDFTTNode& store = _TTable[board.getHash() % TTABLE_SIZE];
    if (store._hash != 0xFFFFFFFFFFFFFFFFLL && store._hash != board.getHash())
        ++_cache_collisions;

    if (board.getHash() != store._hash || store._depth < depth)
    {
        store._hash = board._hash;
        store._lower = INT_MIN;
        store._upper = INT_MAX;
        store._depth = depth;
        if (result < beta)
            store._lower = result;
        if (result > alpha)
            store._upper = result;
    }

    return result;
}



Engine::Engine():
    _ponderer(nullptr),
    _ponderer_best_move(Move()),
    _node_expansions(0),
    _cutoffs(0),
    _cache_hits(0),
    _cache_misses(0),
    _cache_collisions(0),
    _TTable(new MTDFTTNode[TTABLE_SIZE])
{
    
}

Engine::~Engine()
{
    if (nullptr == _TTable)
    {
        delete [] _TTable;
        _TTable = nullptr;
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
    std::cout << "receiving " << move << " " << std::endl;
    _board = _board.applyMove(move);
    std::cout << _board << std::endl;
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
                if ((move            == board._moves[3]) &&
                    (board._moves[6] == board._moves[2]) &&
                    (board._moves[5] == board._moves[1]) &&
                    (board._moves[4] == board._moves[0]))
                {
                    return true;
                }
                return false;
            }),
        actions.end());    
}


}
