#include "TranspositionTable.h"

namespace BixNix {

TranspositionTable::TranspositionTable()
    : _collisions(0), _misses(0), _hits(0), _size(0), _table(nullptr) {}

TranspositionTable::TranspositionTable(const size_t size)
    : _collisions(0),
      _misses(0),
      _hits(0),
      _size(size),
      _table(new MTDFTTNode[size]) {
  for (size_t i = 0; i < _size; ++i) _table[i]._hash = 0xFFFFFFFFFFFFFFFFLL;
}

TranspositionTable::~TranspositionTable() {
  if (nullptr == _table) {
    delete[] _table;
    _table = nullptr;
  }
}

void TranspositionTable::resize(const size_t size) {
  if (nullptr == _table) {
    delete[] _table;
    _table = nullptr;
  }

  _size = size;
  _table = new MTDFTTNode[size];

  for (size_t i = 0; i < _size; ++i) _table[i]._hash = 0xFFFFFFFFFFFFFFFFLL;
}

bool TranspositionTable::get(const ZobristNumber key, const Depth priority,
                             Score& alpha, Score& beta, Score& score) {
  MTDFTTNode& node = _table[key % _size];
  if (node._hash == key && node._depth >= priority) {
    ++_hits;
    if (node._lower >= beta) {
      score = node._lower;
      return true;
    }
    if (node._upper <= alpha) {
      score = node._upper;
      return true;
    }
    if (node._lower == node._upper) {
      score = node._lower;
      return true;
    }
    alpha = std::max(alpha, node._lower);
    beta = std::min(beta, node._upper);
  } else
    ++_misses;

  return false;
}

bool TranspositionTable::set(const ZobristNumber key, const Depth priority,
                             const Score alpha, const Score beta,
                             const Score score) {
  MTDFTTNode& node = _table[key % _size];
  if (node._hash != 0xFFFFFFFFFFFFFFFFLL && node._hash != key) ++_collisions;

  if (key != node._hash || node._depth < priority) {
    node._hash = key;
    node._lower = std::numeric_limits<Score>::min();
    node._upper = std::numeric_limits<Score>::max();
    node._depth = priority;
    if (score < beta) node._lower = score;
    if (score > alpha) node._upper = score;
    return true;
  }
  return false;
}

size_t TranspositionTable::getOccupancy() {
  size_t result(0);
  for (size_t i = 0; i < _size; ++i)
    if (_table[i]._hash != 0xFFFFFFFFFFFFFFFFLL) ++result;
  return result;
}

size_t TranspositionTable::getSize() { return _size; }
}
