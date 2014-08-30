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

int main()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 63);
    auto rand = std::bind(distribution, generator);
    
    BitBoard attacks;

    const int iterations = 100000000;

    time_start();
    for (int i = 0 ; i < iterations ; ++i)
    {
        rand();
        rand();
    }
    double offset = time_stop();
    std::cout << "offset " << offset << " seconds" << std::endl;

    double d;
    
    time_start();
    for (int i = 0 ; i < iterations ; ++i)
    {
        attacks = Knights::GetInstance().getAttacksFrom(Square(rand()));
        attacks |= Knights::GetInstance().getAttacksFrom(Square(rand()));
    }
    d = time_stop();
    std::cout << d - offset << " seconds" << std::endl;
    

    BitBoard knights;
    time_start();
    for (int i = 0 ; i < iterations ; ++i)
    {
        knights = 1LL << rand();
        knights |= 1LL << rand();
        attacks = Knights::GetInstance().getAttacksFrom(knights);        
    }
    d = time_stop();
    std::cout << d - offset << " seconds" << std::endl;

    return 0;
}
