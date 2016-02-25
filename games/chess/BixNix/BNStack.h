#ifndef __BNSTACK_H__
#define __BNSTACK_H__

#include <array>

namespace BixNix
{

template <typename T, int N>    
class BNStack
{
public:
BNStack(): _head_index(0) {}
    BNStack(const BNStack& that):
        _data(that._data),
        _head_index(that._head_index) {}
    BNStack& operator=(const BNStack& that)
    { 
        _data = that._data;
        _head_index = that._head_index;
    }

    ~BNStack() {}

    void push(const T& data) { _data[_head_index++] = data; }
    void pop() { --_head_index; }
    const T& top() { return _data[_head_index - 1]; }
    size_t size() { return _head_index; }
    void clear() { _head_index = 0; }

private:
    std::array<T, N> _data;
    size_t _head_index;
};


}

#endif // __BNSTACK_H__
