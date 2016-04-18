//
// Book.h
//

#ifndef __BOOK_H__
#define __BOOK_H__

#include <tuple>
#include <unordered_map>

#include "Board.h"
#include "Enums.h"
#include "Move.h"

namespace BixNix {

class Book {
 public:
  typedef uint16_t Move;
  typedef uint16_t Weight;
  typedef uint32_t Learn;

  Book();
  ~Book();

  void init(std::string filename);

  void reportMove(Move move);
  BixNix::Move getMove();

 private:
  typedef std::tuple<Move, Weight> Value;

  std::unordered_multimap<ZobristNumber, Value> _positions;
  Board _board;
};
}

#endif  // __BOOK_H__
