#ifndef _MTDF_TTNODE_H_
#define _MTDF_TTNODE_H_

#include <limits>

#include "Enums.h"
#include "Move.h"

namespace BixNix {

class MTDFTTNode {
 public:
  enum class Type : uint8_t { Exact, Lower, Upper };
  MTDFTTNode() : _hash(0xFFFFFFFFFFFFFFFFLL), _score(0), _depth(0), _move(0) {}

  ZobristNumber _hash;
  Score _score;
  Type _type;
  Depth _depth;
  Move::Data _move;
};
}

#endif  // _TRANSPOSITION_NODE_H_
