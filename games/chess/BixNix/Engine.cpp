#include <algorithm>
#include <chrono>
#include <iomanip>
#include <climits>
#include <ctime>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "Logger.h"

#include "Engine.h"
#include "Evaluate.h"

namespace BixNix {

void Engine::end() {
  stopSearch();

  LOG(trace) << _time << " time left";

  auto end_time = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

  LOG(trace) << diff.count() << " seconds";
  LOG(trace) << _node_expansions << " node expansions";
  LOG(trace) << _szL2 << " cutoff nodes";

  LOG(trace) << _szL1 / static_cast<double>(_szL2) << " beta-cutoff ratio";
  LOG(trace) << _node_expansions / static_cast<double>(diff.count())
             << " expansions per second";

  LOG(trace) << (_node_expansions + _ttable._hits) /
                    static_cast<double>(diff.count())
             << " expansions per second counting cache hits";

  LOG(trace) << _ttable._hits << " cache hits";
  LOG(trace) << _ttable._collisions << " cache collisions";
  LOG(trace) << _ttable._misses << " cache misses";
  LOG(trace) << _ttable._hits /
                    static_cast<double>(_ttable._hits + _ttable._misses)
             << " cache hit ratio";

  size_t occupied(_ttable.getOccupancy());
  size_t ttableSize(_ttable.getSize());

  LOG(trace) << (long)occupied << " cache slots occupied";
  LOG(trace) << (long)ttableSize << " cache slots total";
  LOG(trace) << occupied / static_cast<double>(ttableSize) << " occupancy";
}

Move Engine::getMove() {
  stopSearch();   // stop pondering
  startSearch();  // start searching

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
         _best_move_ready.wait_until(lock, endTime)) {
    auto timeSplit = steady_clock::now();
    int diff = duration_cast<microseconds>(timeSplit - startTime).count();
    splits.push_back(diff);

    if (_best_move.getBestPossible()) {
      break;
    }

    if (splits.size() > 3) {
      // assume exponential growth, extend the line
      double ultimate = log(splits[splits.size() - 1]);
      double penultimate = log(splits[splits.size() - 2]);
      double estimate = exp(2 * ultimate - penultimate);

      auto nextMoveEst = startTime + microseconds(static_cast<long>(estimate));
      if (nextMoveEst > endTime) {
        break;
      }
    }
  }

  stopSearch();
  Move move = _best_move;
  _board.applyMove(move);
  _3table.add(_board.getHash());

  LOG(trace) << "engine sending " << move;
  LOG(trace) << "board\n" << _board;

  startSearch();  // start pondering

  return move;
}

void Engine::search() {
  while (!_search_end) {
    _searcherStarted.wait();
    innerSearch();
    _searcherStopped.wait();
  }
}

void Engine::innerSearch() {
  Color myColor = _board.getMover();
  std::vector<Move> actions = _board.getMoves(myColor);
  if (actions.size() == 0) return;

  _best_move = actions[0];
  if (actions.size() == 1) {
    _best_move.setBestPossible(true);
    LOG(trace) << "only move: " << _best_move;
  }
  _best_move_ready.notify_all();
  if (actions.size() == 1) return;

  unsigned int depth = 0;
  for (Move& m : _pv) m = Move(0);

  while (!_search_stop) {
    orderMoves(actions, None, _pv[0]);
    Move bestMoveThisDepth = actions[0];
    Score bestScore = -CHECKMATE;
    Score score;
    if (depth > HEIGHTMAX) return;
    for (Move& m : actions) {
      _board.applyMove(m);
      if (_3table.addWouldTrigger(_board.getHash())) {
        score = DRAW;
      } else {
        _3table.add(_board.getHash());
        score = -negamax(depth);
        _3table.remove(_board.getHash());
      }

      _board.unapplyMove(m);
      if (_search_stop) return;
      if (-DRAW == score) score = DRAW;  // hate to draw
      if (score > bestScore) {
        bestScore = score;
        _pv[0] = m;
        bestMoveThisDepth = m;
        std::stringstream message;
        message << "PV: d" << depth + 1 << " (" << bestScore << ") ";
        for (Move& m : _pv) {
          if (Move(0) == m) break;
          message << m << " ";
        }
        LOG(trace) << message.str();
        if (CHECKMATE == bestScore) {
          _best_move = bestMoveThisDepth;
          _best_move.setBestPossible(true);
          _best_move_ready.notify_all();
          return;
        }
      }
    }

    _best_move = bestMoveThisDepth;
    _best_move_ready.notify_all();

    ++depth;
  }
}

int Engine::negamax(const Depth depth, Score alpha, Score beta,
                    const Depth height) {
  if (_search_stop) return 0;

  Score result(-CHECKMATE);

  if (_ttable.get(_board.getHash(), depth, alpha, beta, result)) return result;
  ++_node_expansions;

  if (0 == depth) {
    result = Evaluate::GetInstance().getEvaluation(_board, _board.getMover());
  } else if (_board.inCheckmate(_board.getMover())) {
    result = -CHECKMATE;
    for (int i = height; i < _pv.size(); ++i) _pv[i] = Move(0);
  } else {
    std::vector<Move> actions(_board.getMoves(_board.getMover()));
    orderMoves(actions, Sort, _pv[height]);

    if (actions.size() == 0 || _board.isDraw100()) {
      result = DRAW;  // stalemate
    } else {
      uint8_t opens = 0;
      for (Move& m : actions) {
        Score score;
        if (result >= beta) {
          _szL1 += opens;
          _szL2 += 1;
          break;
        }

        Color myColor = _board.getMover();
        _board.applyMove(m);
        if (_3table.addWouldTrigger(_board.getHash())) {
          score = DRAW;
        } else {
          _3table.add(_board.getHash());
          score = -negamax(depth - 1, -beta, -alpha, height + 1);
          ++opens;
          _3table.remove(_board.getHash());
        }
        _board.unapplyMove(m);
        if (_search_stop) return 0;

        result = std::max(result, score);
        if (result > alpha) {
          alpha = result;
          _pv[height] = m;
          for (int i = 0; i < height; ++i)
            _pv[height - i - 1] = _board.getPastMove(i);
        }
      }
    }
  }

  _ttable.set(_board.getHash(), result, depth, alpha, beta);
  return result;
}

void Engine::orderMoves(std::vector<Move>& moves, const MoveOrderPolicy policy,
                        const Move& pvMove) {
  bool gotPVMove = false;
  if (pvMove != Move(0)) {
    for (auto& m : moves) {
      if (pvMove == m) {
        std::swap(moves[0], m);
        gotPVMove = true;
        break;
      }
    }
  }

  if (None == policy) return;

  for (auto& m : moves)
    m.score = Evaluate::GetInstance().getEvaluation(m, _board.getMover());

  size_t offset = gotPVMove ? 1 : 0;
  if (Heap == policy) {
    std::make_heap(moves.begin() + offset, moves.end(),
                   [&](const Move& a, const Move& b)
                       -> bool { return a.score < b.score; });
  } else {  // Sort == policy
    std::sort(moves.begin() + offset, moves.end(),
              [&](const Move& a, const Move& b)
                  -> bool { return a.score > b.score; });
  }
}

Engine::Engine()
    : _searcher(nullptr),
      _best_move(Move()),
      _node_expansions(0),
      _szL1(0),
      _szL2(0),
      _search_stop(true),
      _search_end(false) {
  _ttable.resize(TTSIZE);
  _searcher = new std::thread(&Engine::search, this);
}

Engine::~Engine() {
  _search_end = true;
  stopSearch();
  _searcher->join();
  _searcher = nullptr;
}

void Engine::startSearch() {
  if (true == _search_stop) {
    _search_stop = false;
    _searcherStarted.wait();
  }
}

void Engine::stopSearch() {
  if (false == _search_stop) {
    _search_stop = true;
    _searcherStopped.wait();
  }
}

void Engine::init(Color color, float time) {
  srand(std::time(NULL));
  _color = color;
  _time = time;

  _start_time = std::chrono::system_clock::now();

  _board = Board::initial();
  startSearch();
}

void Engine::reportMove(Move move, float time) {
  _time = time;

  stopSearch();
  _board.applyExternalMove(move);
  _3table.add(_board.getHash());
  LOG(trace) << "board\n" << _board;
  startSearch();
}
}
