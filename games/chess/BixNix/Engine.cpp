#include <algorithm>
#include <chrono>
#include <iomanip>
#include <climits>
#include <ctime>
#include <cmath>
#include <functional>
#include <vector>
#include <sstream>

#include "Logger.h"

#include "Engine.h"
#include "Evaluate.h"

namespace BixNix
{

void Engine::end()
{
    stopSearch();

    LOG(trace) << _time << " time left";

    auto end_time = std::chrono::system_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

    LOG(trace) << _node_expansions << " node expansions";
    LOG(trace) << diff.count() << " seconds";
    LOG(trace) << _cutoffs << " cutoffs";

    LOG(trace) << _node_expansions / static_cast<double>(diff.count())
               << " expansions per second";
    LOG(trace) << _cutoffs / static_cast<double>(_node_expansions) 
              << " cutoff ratio";

    LOG(trace) << _ttable._collisions << " cache collisions";
    LOG(trace) << _ttable._hits << " cache hits";
    LOG(trace) << _ttable._misses << " cache misses";
    LOG(trace) << _ttable._hits / static_cast<double>(_ttable._hits + _ttable._misses) 
               << " cache hit ratio";

    size_t occupied(_ttable.getOccupancy());
    size_t ttableSize(_ttable.getSize());

    LOG(trace) << (long)occupied << " cache slots occupied";
    LOG(trace) << (long)ttableSize << " cache slots total";
    LOG(trace) << occupied / static_cast<double>(ttableSize) << " occupancy" << std::endl;
}


Move Engine::getMove()
{
    stopSearch(); // stop pondering
    startSearch(); // start searching

    using namespace std::chrono;
    int runTime = std::min((int)_time / 10, 20);
    auto startTime = steady_clock::now();
    auto endTime = startTime + seconds(runTime);

    std::vector<long> splits;

    std::unique_lock<std::mutex> lock(_cvMutex);

    // Three ways out of this loop:
    // 1. endTime hard timer expires
    // 2. found a checkmate
    // 3. estimated time to finish the next depth exceeds endTime
    while (std::cv_status::no_timeout == 
            _best_move_ready.wait_until(lock, endTime))
    {
        auto timeSplit = steady_clock::now();
        int diff = duration_cast<microseconds>(timeSplit - startTime).count();
        splits.push_back(diff);

        if (_best_move.score >= CHECKMATE)
        {
            break;
        }

        if (splits.size() > 3)
        {
            // assume exponential growth, extend the line
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

    stopSearch();
    Move move = _best_move;
    _board.applyMove(move);
    _3table.add(_board.getHash());

    LOG(trace) << "sending (" << move.score << ") " << move;
    LOG(trace) << "board\n" << _board;

    startSearch(); // start pondering

    return move;
}


void Engine::search()
{
    std::vector<Move> actions(_board.getMoves(_board.getMover()));
    if (actions.size() == 0)
        return;

    _best_move = actions[0];

    unsigned int depth = 0;
    while (!_search_stop)
    {
        if (depth > HEIGHTMAX)
            return;

        for (Move& m : actions)
        {
            _board.applyMove(m);
            if (_3table.addWouldTrigger(_board.getHash()))
            {
                m.score = (- CHECKMATE) + 1;
            } else {
                _3table.add(_board.getHash());
                m.score = - negamax(depth);
                _3table.remove(_board.getHash());                
            }
            _board.unapplyMove(m);
            if (_search_stop)
                return;
        }

        std::sort(actions.begin(), actions.end(),
                  [&](const Move& a, const Move& b) -> bool
                  { return a.score > b.score; });

        _best_move = actions[0];
        _best_move_ready.notify_all();
        _pv[0] = actions[0];
        std::stringstream ss;

        ss << depth << " (" << actions[0].score << ") PV: ";
        for (Move& m : _pv)
        {
            if (Move(0) == m)
                break;
            else
                ss << m << " ";
            m = Move(0);
        }
        LOG(trace) << ss.str();

        ++depth;
    }
}

int Engine::negamax(const unsigned int depth,
                    int alpha,
                    int beta,
                    size_t pvHeight)
{
    ++_node_expansions;
    if (_search_stop)
        return 0;

    int result(-CHECKMATE);

    if (_ttable.get(_board.getHash(), depth, alpha, beta, result))
        return result;

    if (0 == depth)
    {
        result = Evaluate::GetInstance().getEvaluation(_board, _board.getMover());
    }
    else
    {
        std::vector<Move> actions(_board.getMoves(_board.getMover()));
        // PV move reordering, not a full sort
        for (Move& m: actions)
        {
            if (m == _pv[pvHeight])
            {
                std::swap(m, actions[0]);
                break;
            }
        }

        if (actions.size() == 0 && !_board.inCheckmate(_board.getMover()))
        {
            result = 0; // stalemate
        }
        else
        {
            for (Move& m: actions)
            {
                if (result >= beta)
                {
                    ++_cutoffs;
                    break;
                }
                _board.applyMove(m);
                if (_3table.addWouldTrigger(_board.getHash()))
                {
                    m.score = (- CHECKMATE) + 1;
                } else {
                    _3table.add(_board.getHash());
                    m.score = - negamax(depth - 1, -beta, -alpha, pvHeight + 1);
                    _3table.remove(_board.getHash());                    
                }
                _board.unapplyMove(m);
                if (_search_stop)
                    return 0;

                result = std::max(result, m.score);
                if (result > alpha)
                {
                    alpha = result;
                    _pv[pvHeight] = m;
                }
            }
        }
    }

    _ttable.set(_board.getHash(), result, depth, alpha, beta);
    _ttable.set(board.getHash(), result, depth, alpha, beta);
    return result;
}


int Engine::PVS(Board& board,
                const unsigned int depth,
                int alpha,
                int beta,
                size_t pvHeight)
{
    ++_node_expansions;
    if (_search_stop)
        return 0;

    int result(-CHECKMATE);

    if (_ttable.get(board.getHash(), depth, alpha, beta, result))
        return result;

    if (0 == depth)
    {
        result = Evaluate::GetInstance().getEvaluation(std::ref(board), board._toMove);
        // result = quiescent(board, alpha, beta);
        _ttable.set(board.getHash(), result, depth, alpha, beta);
        return result;
    }

    std::vector<Move> actions(board.getMoves(board._toMove));

    for (Move& m: actions)
    {
        if (m == _pv[pvHeight])
        {
            std::swap(m, actions[0]);
            break;
        }
    }

    if (actions.size() == 0)
    {
        TerminalState ts = board.getTerminalState();
        if (WhiteWin == ts && board._toMove == Black)
            result = - CHECKMATE;
        else if (BlackWin == ts && board._toMove == White)
            result = - CHECKMATE;
        else if (WhiteWin == ts && board._toMove == White)
            result = CHECKMATE;
        else if (BlackWin == ts && board._toMove == Black)
            result = CHECKMATE;
        else
            result = 0;

        _ttable.set(board.getHash(), result, depth, alpha, beta);
        return result; // stalemate
    }

    //Board brd(board.applyMove(actions[0]));
    Board brd(board);
    brd.applyMove(actions[0]);
    result = - PVS(brd, depth - 1, -beta, -alpha, pvHeight + 1);
    if (_search_stop)
        return 0;

    if (result > alpha)
    {
        if (result >= beta)
        {
            _ttable.set(board.getHash(), result, depth, alpha, beta);
            return result;
        }
        alpha = result;
    }

    if (actions.size() == 1)
    {
        _ttable.set(board.getHash(), result, depth, alpha, beta);
        return result;
    }

    for (size_t i = 1; i < actions.size(); ++i)
    {
        //Board brd(board.applyMove(actions[i]));
        Board brd(board);
        brd.applyMove(actions[i]);


        int score = - PVS(brd, depth - 1, -alpha - 1, -alpha, pvHeight + 1);
        if (_search_stop)
            return 0;

        if (score > alpha && score < beta)
        {
            score = - PVS(brd, depth - 1, -beta, -alpha, pvHeight + 1);
            if (_search_stop)
                return 0;

            if (score > alpha)
            {
                alpha = score;
                _pv[pvHeight] = actions[i];
            }
        }

        if (score > result)
        {
            if (score >= beta)
            {
                ++_cutoffs;
                _ttable.set(board.getHash(), score, depth, alpha, beta);
                return score;
            }
            result = score;
        }
    }

    _ttable.set(board.getHash(), result, depth, alpha, beta);
    return result;
}


int Engine::quiescent(Board& board,
                      int alpha,
                      int beta)
{
    ++_node_expansions;
    if (_search_stop)
        return 0;

    int result;
    if (_ttable.get(board.getHash(), 0, alpha, beta, result))
        return result;

    std::vector<Move> actions(board.getMoves(board._toMove));
    if (actions.size() == 0)
    {
        if (!board.inCheckmate(board._toMove))
        {
            return 0;
        }
    }

    bool didSomething = false;
    for (Move& m: actions)
    {
        if (!m.getCapturing())
            continue;

        didSomething = true;
        Board brd(board);
        brd.applyMove(m);

        //Board brd(board.applyMove(m));
        m.score = - quiescent(brd, -beta, -alpha);
        if (_search_stop)
            return 0;

        alpha = std::max(alpha, m.score);
        if (m.score >= beta)
            return beta;

    }
    if (!didSomething)
        alpha = Evaluate::GetInstance().getEvaluation(std::ref(board), board._toMove);

    _ttable.set(board.getHash(), alpha, 0, alpha, beta);
    return alpha;
}


Engine::Engine():
    _searcher(nullptr),
    _best_move(Move()),
    _node_expansions(0),
    _cutoffs(0)
{
    _ttable.resize(TTSIZE);
}


Engine::~Engine()
{
    stopSearch();
}


void Engine::startSearch()
{
    if (_searcher == nullptr)
    {
        for (Move& m: _pv)
            m = Move(0);
        _search_stop = false;
        _searcher = new std::thread(&Engine::search, this);
    }
}


void Engine::stopSearch()
{
    _search_stop = true;
    if (_searcher != nullptr)
    {
        if (_searcher->joinable())
        {
            _searcher->join();
        }
        delete _searcher;
        _searcher = nullptr;
    }
}


void Engine::init(Color color, float time)
{
    _color = color;
    _time = time;

    _start_time = std::chrono::system_clock::now();

    stopSearch();
    _board = Board::initial();
    startSearch();
}


void Engine::reportMove(Move move, float time)
{
    LOG(trace) << "receiving " << move;
    _time = time;

    stopSearch();
    _board.applyExternalMove(move);
    _3table.add(_board.getHash());
    LOG(trace) << "board\n" << _board;
}


}
