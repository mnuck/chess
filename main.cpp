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

#include "Queens.h"

int main()
{
    Board b(Board::initial());

    operator<<(std::cout, b.getKingAttacks(Board::White));
    std::cout << std::endl;
    
    //BitBoard atk = b.getKnightAttacks(Board::Black);

//    BitBoard x = 0x000001000000000;
//    operator<<(std::cout, x);
//    std::cout << std::endl;

/*    operator<<(std::cout, smearS(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearN(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearW(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearE(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearNE(x, ~0LL));
    std::cout << std::endl;    

    operator<<(std::cout, smearSE(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearSW(x, ~0LL));
    std::cout << std::endl;

    operator<<(std::cout, smearNW(x, 0x0000FFFFFFFF0000));
    std::cout << std::endl;
    
*/
//    BitBoard y = Queens::GetInstance().getAttacksFrom(x, 0x00FF00000000FF00, 0xFF000000000000FF);
//    operator<<(std::cout, y);
//    std::cout << std::endl;
    
    
    return 0;
}
