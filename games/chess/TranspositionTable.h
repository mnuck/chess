//
// TranspositionTable.h
//

#ifndef __TRANSPOSITIONTABLE_H__
#define __TRANSPOSITIONTABLE_H__

#include "Enums.h"
#include "MTDFTTNode.h"

namespace BixNix
{

class TranspositionTable
{
public:
    TranspositionTable();
    TranspositionTable(const size_t size);
    ~TranspositionTable();
    
    void resize(const size_t size);

    bool get(const ZobristNumber key,
             const unsigned int priority,
             int& alpha, int& beta,
             int& score);

    bool set(const ZobristNumber key, 
             const int score, const unsigned int priority,
             const int alpha, const int beta);
    
    size_t getOccupancy();
    size_t getSize();

    unsigned long long _collisions;
    unsigned long long _misses;
    unsigned long long _hits;

private:
    size_t _size;
    MTDFTTNode* _table;
};

}

#endif // __TRANSPOSITIONTABLE_H__
