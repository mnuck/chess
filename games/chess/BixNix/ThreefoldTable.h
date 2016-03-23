#ifndef __THREEFOLDTABLE_H__
#define __THREEFOLDTABLE_H__

#include <unordered_map>

#include "Enums.h"

namespace BixNix {

class ThreefoldTable {
 public:
  void add(const ZobristNumber key);
  void remove(const ZobristNumber key);
  bool addWouldTrigger(const ZobristNumber key) const;

 private:
  std::unordered_map<ZobristNumber, uint8_t> _table;
};
}

#endif  // __THREEFOLDTABLE_H__
