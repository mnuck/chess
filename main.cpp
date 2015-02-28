#include <chrono>
#include <random>
#include <vector>
#include <string>

#include "Board.h"
#include "test.h"

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
    if (2 == argc)
    {
        if ("--test" == std::string(argv[1]))
        {
            if (test())
            {
                std::cout << "All Tests Pass!" << std::endl;
                return 0;
            } else {
                std::cout << "Some Tests Fail!" << std::endl;
                return 1;
            }
            return test() ? 0 : 1;
        }
    }
    

    BixNix::Board board = BixNix::Board::parse("tests/WhiteCanEnPassant");
    std::cout << board << std::endl;
    board = board.applyMove(BixNix::Move(36, 43));
    std::cout << board << std::endl;
    

    //std::random_device rd;
    //std::default_random_engine el(rd());
    //auto rand = std::uniform_int_distribution<int>(0, 1);

/*    for (BixNix::Move& m: moves)
    {
        std::cout << m << std::endl;
        BixNix::Board b = board.applyMove(m);
        std::cout << b << std::endl;
    }
*/  
/*    BixNix::Color color = BixNix::White;
    
    time_start();
    for (int i = 0; i < 300000; ++i)
        std::vector<BixNix::Move> moves = board.getMoves(color);

    std::cout << time_stop() << std::endl;
*/  
    /*

    
    
    int i = 0;
    for (; moves.size() > 0 && board.good(); ++i)
    {
        rand = std::uniform_int_distribution<int>(0, moves.size() - 1);
        size_t index = rand(el);
        board = board.applyMove(moves[index]);

        color = BixNix::Color(1 - color);
        moves = board.getMoves(color);
    }
    std::cout << i << std::endl;
    std::cout << board << std::endl;
    */

    return 0;
}
