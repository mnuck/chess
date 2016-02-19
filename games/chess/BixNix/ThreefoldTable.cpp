#include "ThreefoldTable.h"

namespace BixNix
{

void ThreefoldTable::add(const ZobristNumber key)
{
    _table.insert(key);
}

void ThreefoldTable::remove(const ZobristNumber key)
{
    _table.erase(key);
}

bool ThreefoldTable::addWouldTrigger(const ZobristNumber key) const
{
    return (_table.count(key) > 1);
}
    
}
