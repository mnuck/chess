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
  LOG(trace) << _node_expansions / (900 - _time) << " expansions per second";

  LOG(trace) << (_node_expansions + _ttable._hits) / (900 - _time)
             << " expansions per second counting cache hits";

  LOG(trace) << _ttable._hits << " cache hits";
  LOG(trace) << _ttable._collisions << " cache collisions";
  LOG(trace) << _ttable._misses << " cache misses";
  LOG(trace) << _ttable._hits /
                    static_cast<double>(_ttable._hits + _ttable._misses)
             << " cache hit ratio";

  size_t occupied(_ttable.getOccupancy());
  size_t ttableSize(_ttable.getSize());

  LOG(trace) << occupied / static_cast<double>(ttableSize) << " occupancy";

  LOG(trace) << _board._ms.maxHead() << " MoveStack Max";
}

Move Engine::getMove() {
  startSearch();

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
  unsigned int depth = 0;
  _board._ms.newFrame();
  _board.getMoves(myColor);
  _board._ms.popTo(std::remove_if(_board._ms.begin(), _board._ms.end(),
                                  [&](Move& move) -> bool {
    _board.applyMove(move);
    bool bad = _board.inCheck(myColor);
    _board.unapplyMove(move);
    return bad;
  }));
  if (_board._ms.size() == 0) goto InnerSearchDone;

  _best_move = _board._ms[0];
  if (_board._ms.size() == 1) {
    _best_move.setBestPossible(true);
    LOG(trace) << "only move: " << _best_move;
  }
  _best_move_ready.notify_all();
  if (_board._ms.size() == 1) goto InnerSearchDone;

  for (Move& m : _pv) m = 0;

  while (!_search_stop) {
    emplaceFirstMove(_pv[0], Move(0));
    Move bestMoveThisDepth = _board._ms[0];
    Score bestScore = std::numeric_limits<Score>::min();
    Score score = std::numeric_limits<Score>::min();
    if (depth > (HEIGHTMAX - 32)) goto InnerSearchDone;
    LOG(trace) << "********* DEPTH " << depth;
    LOG(trace) << "Threefold Table";
    for (auto it : _3table._table) {
      LOG(trace) << it.first << ": " << (int)it.second;
    }
    for (Move& m : _board._ms) {
      _board.applyMove(m);
      LOG(trace) << "hash: " << _board.getHash();
      if (_3table.addWouldTrigger(_board.getHash())) {
        score = DRAW;
      } else {
        _3table.add(_board.getHash());
        score = -negamax(depth);
        _3table.remove(_board.getHash());
      }

      _board.unapplyMove(m);

      LOG(trace) << m << ": " << score;

      if (_search_stop) goto InnerSearchDone;
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
          goto InnerSearchDone;
        }
      }
    }

    _best_move = bestMoveThisDepth;
    _best_move_ready.notify_all();

    ++depth;
  }
InnerSearchDone:
  _board._ms.popFrame();
}

Score Engine::negamax(const Depth depth, Score alpha, Score beta,
                      const Depth height) {
  if (_search_stop) return 0;

  Score alphaParent = alpha;
  Score result = std::numeric_limits<Score>::min();
  Score score = std::numeric_limits<Score>::min();
  Move ttMove = 0;
  Move& pvMove = _pv[height];
  Color myColor = _board.getMover();
  uint8_t opens = 0;
  bool needToPop = false;
  bool firstMove = true;

  if (_ttable.get(_board.getHash(), depth, alpha, beta, result, ttMove))
    return result;

  ++_node_expansions;

  if (result >= beta) {
    _szL1 += opens;
    _szL2 += 1;
    goto NegamaxDone;
  }

  if (0 == depth) {
    result = Evaluate::GetInstance().getEvaluation(_board, myColor);
    goto NegamaxDone;
  }

  if (_board.inCheckmate(myColor)) {
    result = -CHECKMATE;
    for (int i = height; i < _pv.size(); ++i) _pv[i] = 0;
    goto NegamaxDone;
  }

  if (_board.isDraw100()) {
    result = DRAW;  // stalemate
    goto NegamaxDone;
  }

  _board._ms.newFrame();
  needToPop = true;
  _board.getMoves(myColor);

  emplaceFirstMove(pvMove, ttMove);

  for (auto it = _board._ms.begin(); it != _board._ms.end(); ++it) {
    auto& m = *it;
    score = std::numeric_limits<Score>::min();

    _board.applyMove(m);
    if (!_board.inCheck(myColor)) {
      if (_3table.addWouldTrigger(_board.getHash())) {
        // assume my opponent WANTS to tie
        if (height % 2 == 0)
          score = DRAW;
        else
          score = CHECKMATE;
      } else {
        _3table.add(_board.getHash());
        score = -negamax(depth - 1, -beta, -alpha, height + 1);
        ++opens;
        _3table.remove(_board.getHash());
      }
    }
    _board.unapplyMove(m);
    if (_search_stop) {
      result = 0;
      goto NegamaxDone;
    }

    result = std::max(result, score);
    if (result >= beta) {
      _szL1 += opens;
      _szL2 += 1;
      goto NegamaxDone;
    }
    if (result > alpha) {
      ttMove = m;
      pvMove = m;
      alpha = result;
      for (int i = 0; i < height; ++i)
        _pv[height - i - 1] = _board.getPastMove(i);
    }
    if (firstMove) {
      firstMove = false;
      std::sort(_board._ms.begin() + 1, _board._ms.end(),
                [&](const Move& a, const Move& b)
                    -> bool { return a.score > b.score; });
    }
  }

  if (0 == opens) {
    result = DRAW;
  }

NegamaxDone:
  if (needToPop) _board._ms.popFrame();
  _ttable.set(_board.getHash(), depth, alphaParent, beta, result, ttMove);
  return result;
}

void Engine::emplaceFirstMove(const Move& pvMove, const Move& ttMove) {
  auto begin = _board._ms.begin();
  auto end = _board._ms.end();
  auto pvIterator = end;
  auto ttIterator = end;
  auto bestIterator = begin;
  for (auto it = begin; it != end; ++it) {
    (*it).score = Evaluate::GetInstance().getEvaluation(*it, _board.getMover());
    if ((*it).score > (*bestIterator).score) bestIterator = it;
    if (pvMove == *it) pvIterator = it;
    if (ttMove == *it) ttIterator = it;
  }

  if (pvIterator != end)
    std::swap(*begin, *pvIterator);
  else if (pvIterator != _board._ms.end())
    std::swap(*begin, *ttIterator);
  else
    std::swap(*begin, *bestIterator);
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
    _ttable.clear();
  }
}

void Engine::init(Color color, float time) {
  srand(std::time(NULL));
  _color = color;
  _time = time;

  _start_time = std::chrono::system_clock::now();

  _board = Board::initial();
  _3table.add(_board.getHash());
}

void Engine::reportMove(Move move, float time) {
  _time = time;

  _board.applyExternalMove(move);
  _3table.add(_board.getHash());
  LOG(trace) << "board:\n" << _board;
}
}
