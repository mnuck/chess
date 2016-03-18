//
// TranspositionTable.h
//
// http://www.gamedev.net/topic/503234-transposition-table-question/#entry4282970

#ifndef __TRANSPOSITIONTABLE_H__
#define __TRANSPOSITIONTABLE_H__

#include "Enums.h"
#include "MTDFTTNode.h"

namespace BixNix {

class TranspositionTable {
 public:
  TranspositionTable();
  TranspositionTable(const size_t size);
  ~TranspositionTable();

  void resize(const size_t size);
  void clear();

  bool get(const ZobristNumber key, const Depth priority, Score& alpha,
           Score& beta, Score& score, Move& move);

  bool set(const ZobristNumber key, const Depth priority, const Score alpha,
           const Score beta, const Score score, const Move& move);

  size_t getOccupancy();
  size_t getSize();

  uint64_t _collisions;
  uint64_t _misses;
  uint64_t _hits;

 private:
  size_t _size;
  MTDFTTNode* _table;
};
}

#endif  // __TRANSPOSITIONTABLE_H__
