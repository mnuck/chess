#include "ThreefoldTable.h"

namespace BixNix {

void ThreefoldTable::add(const ZobristNumber key) { _table[key] += 1; }

void ThreefoldTable::remove(const ZobristNumber key) {
  auto it = _table.find(key);
  if (it == _table.end()) return;
  it->second -= 1;
  if (it->second == 0) _table.erase(it);
}

bool ThreefoldTable::addWouldTrigger(const ZobristNumber key) const {
  auto it = _table.find(key);
  if (it == _table.end()) return false;
  return it->second > 1;
}
}
