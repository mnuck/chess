#include <chrono>
#include <functional>
#include <iostream>
#include <random>

#include "Board.h"
#include "Knights.h"

using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

steady_clock::time_point start;
steady_clock::time_point end;


void time_start()
{
    start = steady_clock::now();
}

double time_stop()
{
    end = steady_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(end - start);
    return time_span.count();
}

#include <vector>

#include "Board.h"
#include "Move.h"

int main()
{
    std::random_device rd;
    std::default_random_engine el(rd());
    auto rand = std::uniform_int_distribution<int>(0, 1);    

    Board board(Board::initial());
    std::cout << board << std::endl;

    Board::Color color = Board::White;
    std::vector<Move> moves;
    
    int i = 0;
    
    moves = board.getMoves(color);
    while (moves.size() > 0)
    {
        rand = std::uniform_int_distribution<int>(0, moves.size() - 1);
        size_t index = rand(el);
        board = board.applyMove(moves[index]);
        std::cout << board << std::endl;

        color = Board::Color(abs(1 - color));
        moves = board.getMoves(color);
        ++i;
    }
    std::cout << i << std::endl;

    return 0;
}
