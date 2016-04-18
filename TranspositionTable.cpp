#include "TranspositionTable.h"

namespace BixNix {

TranspositionTable::TranspositionTable()
    : _collisions(0), _misses(0), _hits(0), _size(0), _table(nullptr) {}

TranspositionTable::TranspositionTable(const size_t size)
    : _collisions(0),
      _misses(0),
      _hits(0),
      _size(size),
      _maxOccupancy(0),
      _table(new MTDFTTNode[size]) {
  clear();
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

  clear();
}

void TranspositionTable::clear() {
  size_t occupancy = 0;
  for (size_t i = 0; i < _size; ++i)
    if (_table[i]._hash != 0xFFFFFFFFFFFFFFFFLL) ++occupancy;
  _maxOccupancy = std::max(_maxOccupancy, occupancy);

  for (size_t i = 0; i < _size; ++i) _table[i]._hash = 0xFFFFFFFFFFFFFFFFLL;
}

bool TranspositionTable::get(const ZobristNumber key, const Depth priority,
                             Score& alpha, Score& beta, Score& score,
                             Move& move) {
  MTDFTTNode& node = _table[key % _size];
  if (node._hash == key && node._depth >= priority) {
    ++_hits;
    move = node._move;
    switch (node._type) {
      case MTDFTTNode::Type::Exact:
        score = node._score;
        return true;
        break;
      case MTDFTTNode::Type::Lower:
        alpha = std::max(alpha, node._score);
        break;
      case MTDFTTNode::Type::Upper:
        beta = std::min(beta, node._score);
        break;
    }
    if (alpha >= beta) {
      score = node._score;
      return true;
    }
  } else
    ++_misses;

  return false;
}

bool TranspositionTable::set(const ZobristNumber key, const Depth priority,
                             const Score alpha, const Score beta,
                             const Score score, const Move& move) {
  MTDFTTNode& node = _table[key % _size];
  if (node._hash != 0xFFFFFFFFFFFFFFFFLL && node._hash != key) ++_collisions;

  if (key != node._hash || node._depth < priority) {
    node._hash = key;
    node._score = score;
    node._depth = priority;
    node._move = move;
    if (score <= alpha)
      node._type = MTDFTTNode::Type::Upper;
    else if (score >= beta)
      node._type = MTDFTTNode::Type::Lower;
    else
      node._type = MTDFTTNode::Type::Exact;
    return true;
  }
  return false;
}

size_t TranspositionTable::getOccupancy() { return _maxOccupancy; }

size_t TranspositionTable::getSize() { return _size; }
}
