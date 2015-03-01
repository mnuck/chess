//
// Engine.h
//

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Enums.h"
#include "Board.h"

namespace BixNix
{

class Engine
{
public:
    Engine();
    ~Engine();

    void init(Color color);
    void end();

    void reportTimeLeft(float time);
    void reportMove(Move move);
    
    Move getMove();

private:
    Board _board;
    Color _color;
    float _time;
};

}



#endif // __ENGINE_H__
