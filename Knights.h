#ifndef _KNIGHTS_H_
#define _KNIGHTS_H_

#include <array>
#include "Board.h"

namespace BixNix {

class Knights {
 public:
  static Knights& GetInstance();
  virtual ~Knights();
  BitBoard getAttacksFrom(Square index);
  BitBoard getAttacksFrom(BitBoard knights, BitBoard obstructions);

 protected:
  Knights();
  BitBoard generateAttacksFrom(Square index);
  std::array<BitBoard, 64> _attacks;
};
}

#endif  // _KNIGHTS_H_
