#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <vector>

#include "Engine.h"

namespace BixNix
{

Engine::Engine():
    _ponderer(nullptr),
    _ponderer_best_move(Move(0,0)),
    _cache_hits(0),
    _cache_misses(0),
    _tTable(nullptr)
{
    _tTable = new TranspositionNode[TTABLE_SIZE];
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

void Engine::end() 
{
    auto end_time = std::chrono::system_clock::now();
    auto diff = 
        std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

    std::cout << _heuristic_runs << " heuristic runs" << std::endl
              << diff.count() << " seconds" << std::endl
              << _heuristic_runs / diff.count() << " runs per sec" << std::endl;
    std::cout << _cache_hits << " cache hits\n"
              << _cache_misses << " cache misses\n"
              << _cache_hits / static_cast<double>(_cache_hits + _cache_misses) << " ratio" << std::endl;

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
                m.score = minimax(b, Color(1 -_color), depth, Min);
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

            ++depth;
        }
    }
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
    std::cout << move << std::endl;
    _board = _board.applyMove(move);
    
    return move;
}


float Engine::minimax(Board& board, 
                      const Color toMove,
                      const unsigned int depth,
                      const MinimaxPlayer player,
                      float alpha,
                      float beta)
{
    if (_ponderer_done)
        return 0.0;

    if (_ponderer_needs_new_board)
    {
        return 0.0;
    }

    if (0 == depth)
    {
        return heuristic(board);
    }

    float result;
    if (Max == player)
        result = -100000;
    else
        result =  100000;

    std::vector<Move> actions = board.getMoves(toMove);
    trimTrifoldRepetition(board, actions);

    if (actions.size() == 0)
    {
        return heuristic(board);
    }

    for (Move& m : actions)
    {
        Board b(board.applyMove(m));
        TranspositionNode node = _tTable[b.getHash() % TTABLE_SIZE];
        if (node._hash == b.getHash())
            m.score = node._score;
        else
            m.score = heuristic(b);
    }

    if (Max == player)
    {
        std::sort(actions.begin(), actions.end(),
                  [&](const Move& a, const Move& b) -> bool
                  { return a.score > b.score; });
    } else {
        std::sort(actions.begin(), actions.end(),
                  [&](const Move& a, const Move& b) -> bool
                  { return a.score < b.score; });
    }

    for (Move& m : actions)
    {
        Board resultBoard(board.applyMove(m));
        float value;
        
        size_t index = resultBoard.getHash() % TTABLE_SIZE;
        
        if (_tTable[index]._hash == resultBoard.getHash() && 
            _tTable[index]._depth >= depth)
        { // cache hit
            ++_cache_hits;
            value =  _tTable[index]._score;
        } 
        else 
        {
            ++_cache_misses;
            value = 
                minimax(
                    resultBoard,
                    Color(1 - toMove),
                    depth - 1,
                    MinimaxPlayer(1 - player),
                    alpha,
                    beta);

            if (_tTable[index]._hash != resultBoard.getHash() ||
                _tTable[index]._depth < depth)
            {
                _tTable[index]._hash = resultBoard.getHash();
                _tTable[index]._depth = depth;
                _tTable[index]._score = value;
            }
        }

        if (Max == player)
        {
            result = std::max(result, value);
            alpha  = std::max(alpha, result);
        } else {
            result = std::min(result, value);
            beta   = std::min(beta, result);
        }
        if (beta <= alpha)
        {
            break;
        }        
    }
    return result;    
}


float Engine::heuristic(Board& board)
{
    ++_heuristic_runs;

    if ((board._toMove == _color) && board.inCheckmate(_color))
    {
        return -100000;
    }
    if ((board._toMove == Color(1 - _color)) && board.inCheckmate(Color(1 - _color)))
    {
        return 100000;
    }
    
    float pawnValue = 1;
    float knightValue = 3;
    float bishopValue = 3;
    float rookValue = 5;
    float queenValue = 9;    
    
    int pawnDiff = 
        __builtin_popcountll(board._pieces[Pawn] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Pawn] & board._colors[1 - _color]);

    int knightDiff = 
        __builtin_popcountll(board._pieces[Knight] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Knight] & board._colors[1 - _color]);

    int bishopDiff = 
        __builtin_popcountll(board._pieces[Bishop] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Bishop] & board._colors[1 - _color]);

    int rookDiff = 
        __builtin_popcountll(board._pieces[Rook] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Rook] & board._colors[1 - _color]);

    int queenDiff = 
        __builtin_popcountll(board._pieces[Queen] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Queen] & board._colors[1 - _color]);    

    return
        pawnValue * pawnDiff +
        knightValue * knightDiff +
        bishopValue * bishopDiff +
        rookValue * rookDiff +
        queenValue * queenDiff;
}

void Engine::trimTrifoldRepetition(Board& board, std::vector<Move>& actions)
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
