#ifndef _MTDF_TTNODE_H_
#define _MTDF_TTNODE_H_

#include <limits>

#include "Enums.h"
#include "Move.h"

namespace BixNix {

class MTDFTTNode {
 public:
  MTDFTTNode()
      : _hash(0xFFFFFFFFFFFFFFFFLL),
        _lower(std::numeric_limits<Score>::min()),
        _upper(std::numeric_limits<Score>::max()),
        _depth(0) {}

  ZobristNumber _hash;
  Score _lower;
  Score _upper;
  Depth _depth;
};
}

#endif  // _TRANSPOSITION_NODE_H_
