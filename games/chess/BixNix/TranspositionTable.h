//
// TranspositionTable.h
//

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

  bool get(const ZobristNumber key, const Depth priority, Score& alpha,
           Score& beta, Score& score);

  bool set(const ZobristNumber key, const Score score, const Depth priority,
           const Score alpha, const Score beta);

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
