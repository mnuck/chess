#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "Board.h"

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


int main(int argc, char* argv[])
{
    Board board(Board::initial());
    if (2 == argc)
    {
        board = Board::parse(argv[1]);
    }
    std::cout << board << std::endl;

    std::random_device rd;
    std::default_random_engine el(rd());
    auto rand = std::uniform_int_distribution<int>(0, 1);    

    Color color = White;
    std::vector<Move> moves;

    moves = board.getMoves(color);

    time_start();
    for (int i = 0; i < 10000000; ++i)
        Board b = board.applyMove(moves[2]);

    std::cout << time_stop() << std::endl;


    
/*
    for (int i = 0; moves.size() > 0 && board.good(); ++i)
    {
        rand = std::uniform_int_distribution<int>(0, moves.size() - 1);
        size_t index = rand(el);
        board = board.applyMove(moves[index]);

        color = Color(abs(1 - color));
        moves = board.getMoves(color);
        std::cout << i << std::endl;
        std::cout << board << std::endl;
    }
*/
    return 0;
}
