#ifndef __FRAMED_STACK_H__
#define __FRAMED_STACK_H__

#include <array>
#include <cstddef>
#include <iterator>

template <typename T, size_t Size, size_t MaxFrames>
class FramedStackIter;

template <typename T, size_t Size, size_t MaxFrames>
class FramedStack {
 public:
  friend class FramedStackIter<T, Size, MaxFrames>;

  typedef FramedStackIter<T, Size, MaxFrames> iterator;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;

  FramedStack() : _dataHead(0), _fpHead(0), _maxDataHead(0) {
    _framePointer[0] = 0;
  }
  ~FramedStack() {}

  void push(value_type data) {
    _data[_dataHead++] = data;
    _maxDataHead = std::max(_maxDataHead, _dataHead);
  }
  void popTo(iterator it) { _dataHead = it._index; }

  void newFrame() { _framePointer[++_fpHead] = _dataHead; }
  void popFrame() { _dataHead = _framePointer[_fpHead--]; }

  size_t size() { return _dataHead - _framePointer[_fpHead]; }
  bool empty() { return _dataHead == _framePointer[_fpHead]; }
  size_t maxHead() { return _maxDataHead; }

  value_type& operator[](size_t n) { return _data[_framePointer[_fpHead] + n]; }

  iterator begin() { return iterator(*this, _framePointer[_fpHead]); }
  iterator end() { return iterator(*this, _dataHead); }

 protected:
  std::array<value_type, Size> _data;
  std::array<size_t, MaxFrames> _framePointer;
  size_t _dataHead;
  size_t _fpHead;
  size_t _maxDataHead;
};

template <typename T, size_t Size, size_t MaxFrames>
FramedStackIter<T, Size, MaxFrames> operator+(
    typename FramedStackIter<T, Size, MaxFrames>::difference_type n,
    FramedStackIter<T, Size, MaxFrames>& rhs) {
  FramedStackIter<T, Size, MaxFrames> result(rhs);
  result += n;
  return result;
}

template <typename T, size_t Size, size_t MaxFrames>
class FramedStackIter {
 public:
  friend class FramedStack<T, Size, MaxFrames>;

  typedef typename FramedStack<T, Size, MaxFrames>::value_type value_type;
  typedef typename FramedStack<T, Size, MaxFrames>::difference_type
      difference_type;
  typedef typename FramedStack<T, Size, MaxFrames>::pointer pointer;
  typedef typename FramedStack<T, Size, MaxFrames>::reference reference;
  typedef typename std::random_access_iterator_tag iterator_category;

  FramedStackIter(const FramedStackIter& rhs)
      : _fs(rhs._fs), _index(rhs._index) {}

  FramedStackIter(FramedStack<T, Size, MaxFrames>& fs, size_t index)
      : _fs(fs), _index(index) {}

  FramedStackIter& operator=(const FramedStackIter& rhs) {
    _index = rhs._index;
    return *this;
  }

  value_type& operator*() { return _fs._data[_index]; }

  FramedStackIter& operator++() {
    ++_index;
    return *this;
  }

  FramedStackIter operator++(int) {
    FramedStackIter result(*this);
    ++_index;
    return result;
  }

  FramedStackIter& operator--() {
    --_index;
    return *this;
  }

  FramedStackIter operator--(int) {
    FramedStackIter result(*this);
    --_index;
    return result;
  }

  FramedStackIter& operator+=(difference_type n) {
    _index += n;
    return *this;
  }

  FramedStackIter& operator-=(difference_type n) {
    _index -= n;
    return *this;
  }

  FramedStackIter operator+(difference_type n) {
    FramedStackIter result(*this);
    result += n;
    return result;
  }

  FramedStackIter operator-(difference_type n) { return *this + (-n); }

  difference_type operator-(const FramedStackIter& rhs) {
    difference_type x = _index;
    difference_type y = rhs._index;
    return x - y;
  }

  template <typename xT, size_t xSize, size_t xMaxFrames>
  friend FramedStackIter<xT, xSize, xMaxFrames> operator+(
      typename FramedStackIter<xT, xSize, xMaxFrames>::difference_type n,
      FramedStackIter<xT, xSize, xMaxFrames>& rhs);

  value_type& operator[](difference_type n) { return *(*this + n); }
  bool operator==(const FramedStackIter& rhs) { return _index == rhs._index; }
  bool operator!=(const FramedStackIter& rhs) { return !(*this == rhs); }
  bool operator<(const FramedStackIter& rhs) { return _index < rhs._index; }
  bool operator>(const FramedStackIter& rhs) { return _index > rhs._index; }
  bool operator<=(const FramedStackIter& rhs) { return !(*this > rhs); }
  bool operator>=(const FramedStackIter& rhs) { return !(*this < rhs); }

 protected:
  FramedStack<T, Size, MaxFrames>& _fs;
  size_t _index;
};

#endif  // __FRAMED_STACK_H__
