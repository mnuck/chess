#include <array>
#include <random>

#include "Zobrist.h"

namespace BixNix
{

Zobrist& Zobrist::GetInstance()
{
    static Zobrist instance;
    return instance;
}


Zobrist::Zobrist()
{
    std::mt19937_64 getRand;
    getRand.seed(1234567890LL);
    
    _blackToMove = getRand();
    _WQCastle = getRand();
    _WKCastle = getRand();
    _BQCastle = getRand();
    _BKCastle = getRand();

    for (ZobristNumber& z: _pieces)
        z = getRand();
        
    for (ZobristNumber& z: _epFile)
        z = getRand();
}


ZobristNumber Zobrist::getZobrist(Color color, Piece piece, Square square)
{
    size_t offset =  (color * 364) + (piece * 64) + square;
    return _pieces[offset];    
}


ZobristNumber Zobrist::getEPFile(Square square)
{
    size_t file = square % 8;
    return _epFile[file];
}


}
