#include <vector>

#include "Engine.h"

namespace BixNix
{

Engine::Engine()
{
    
}

Engine::~Engine()
{
        
}

void Engine::init(Color color)
{
    srand(time(NULL));
    _board = Board::initial();
    _color = color;
}

void Engine::end() 
{
    
}

void Engine::reportTimeLeft(float time)
{
    _time = time;
}

void Engine::reportMove(Move move)
{
    _board = _board.applyMove(move);
}

Move Engine::getMove()
{
    std::vector<Move> potentialMoves = _board.getMoves(_color);
    Move move = potentialMoves[rand() % potentialMoves.size()];
    _board = _board.applyMove(move);
    return move;
}

}
