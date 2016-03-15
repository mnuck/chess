//
// Engine.h
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <array>
#include <atomic>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <memory>
#include <thread>

#include "Rendezvous.h"

#include "Enums.h"
#include "Board.h"
#include "TranspositionTable.h"
#include "ThreefoldTable.h"

namespace BixNix {

class Engine {
 public:
  Engine();
  ~Engine();

  void init(Color color, float time);
  void end();

  void reportMove(Move move, float time);
  Move getMove();

 private:
  int negamax(const int depth, int alpha = -CHECKMATE, int beta = CHECKMATE,
              const int height = 1);

  void startSearch();
  void stopSearch();
  void search();
  void innerSearch();

  Board _board;
  Color _color;
  float _time;

  static const int HEIGHTMAX = 16;

  std::thread* _searcher;
  std::atomic_bool _search_stop;
  std::atomic_bool _search_end;
  std::condition_variable _best_move_ready;
  std::mutex _cvMutex;

  Rendezvous _searcherStarted;
  Rendezvous _searcherStopped;

  Move _best_move;

  uint64_t _node_expansions;

  // https://chessprogramming.wikispaces.com/Sier%C5%BCant#Cutratio
  uint64_t _szL1;
  uint64_t _szL2;

  std::chrono::time_point<std::chrono::system_clock> _start_time;

  static const int TTSIZE = 33554432;
  TranspositionTable _ttable;
  ThreefoldTable _3table;

  std::array<Move, 16> _pv;

  void collectStats();
  std::vector<float> _timePassed;
  std::vector<float> _timeLeft;
  std::vector<uint64_t> _expansions;

  std::vector<size_t> _ttSize;
  std::vector<size_t> _ttOccupancy;
  std::vector<uint64_t> _ttMisses;
  std::vector<uint64_t> _ttHits;
  std::vector<uint64_t> _ttCollisions;

  bool _pvFirst;
  int _sortHashNeither;
};
}

#endif  // __ENGINE_H__
