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
    Board b(Board::initial());

    for (Move& m: b.getMoves(Board::Black))
    {
        std::cout << m << std::endl;
    }
    
    return 0;
}
