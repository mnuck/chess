#ifndef __MOVE_H__
#define __MOVE_H__

#include <iostream>

#include "BitBoard.h"
#include "Enums.h"

namespace BixNix
{

class Move
{
    friend std::ostream& operator<<(std::ostream& lhs, const Move& rhs);


public:
    Move(Square source, Square target, Piece piece=Queen): 
    _source(source), _target(target), _piece(piece) {}
    
    void setSource(Square& s) { _source = s; }
    void setTarget(Square& t) { _target = t; }
    void setPiece(Piece& p)   { _piece = p; }
    Square getSource() { return _source; }
    Square getTarget() { return _target; }
    Piece  getPiece()  { return _piece; }

    bool operator==(const Move& rhs);

private:
    Square _source;
    Square _target;
    Piece _piece;
};

std::ostream& operator<<(std::ostream& lhs, const Move& rhs);

}


#endif // __MOVE_H_
