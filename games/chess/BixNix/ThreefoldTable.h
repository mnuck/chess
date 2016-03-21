#ifndef __THREEFOLDTABLE_H__
#define __THREEFOLDTABLE_H__

#include <unordered_set>

#include "Enums.h"

namespace BixNix {

class ThreefoldTable {
 public:
  void add(const ZobristNumber key);
  void remove(const ZobristNumber key);
  bool addWouldTrigger(const ZobristNumber key) const;
  void dumpTable() const;

 private:
  std::unordered_multiset<ZobristNumber> _table;
};
}

#endif  // __THREEFOLDTABLE_H__
