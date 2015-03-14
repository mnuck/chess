#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <sstream>

#include "Board.h"
#include "test.h"

using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

steady_clock::time_point start;
steady_clock::time_point end;

using namespace BixNix;

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
/*  
    std::string kiwipete = 
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    std::stringstream kp(kiwipete);
    Board b(Board::parseEPD(kp));
*/
//    b = b.applyMove(Move(35, 52));
//    b = b.applyMove(Move(16, 9));
/*
    std::cout << (unsigned long long)b.perft(1) << std::endl;
    std::cout << b << std::endl;
    
    std::cout << b.inCheck(White) << std::endl;
*/  
/*
    for (const Move& m: b.getMoves(White))
    {
        Board brd(b.applyMove(m));
        std::cout << m << ": " << (unsigned long long)brd.perft(0) << std::endl;
    }


    for (int depth = 0; depth < 6; ++depth)
    {
        std::cout << depth << ": " 
                  << (unsigned long long)b.perft(depth) << std::endl;
    }
*/

    std::string p3 = 
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
    std::stringstream p3ss(p3);
    Board p3b(Board::parseEPD(p3ss));
    std::cout << p3b << std::endl;
    for (int depth = 0; depth < 8; ++depth)
    {
        std::cout << depth << ": " 
                  << (unsigned long long)p3b.perft(depth) << std::endl;
    }
  
/*
    Board a(Board::initial());
    for (int depth = 0; depth < 8; ++depth)
    {
        std::cout << depth << ": " 
                  << (unsigned long long)a.perft(depth) << std::endl;
    }
*/   
    /*

    std::string kiwipete(
        "R...K..R"
        "P.PPQPB."
        "BN..PNP."
        "...pn..."
        ".P..p..."
        "..n..q.P"
        "pppbbppp"
        "r...k..r");    
    std::stringstream kp(kiwipete, std::ios_base::in);
    Board b(Board::parse(kp));
    */

//    std::cout << (unsigned long long)b.perft(3) << std::endl;
/*    std::cout << b.captures << std::endl;
    std::cout << b.ep << std::endl;
    std::cout << b.castles << std::endl;
    std::cout << b.promo << std::endl;
    std::cout << b.checks << std::endl;
    std::cout << b.checkmates << std::endl;*/

//    std::cout << (unsigned long long)b.perft(3) << std::endl;


/*    auto moves = b.getMoves(White);
    for (const Move& m : moves)
        std::cout << m << std::endl;
*/  

   

/*
    BixNix::Board a = BixNix::Board::initial();
    a = a.applyMove(BixNix::Move(9, 25));
    a = a.applyMove(BixNix::Move(49, 33));

    BixNix::Board b = BixNix::Board::initial();
    b = b.applyMove(BixNix::Move(9, 17));
    b = b.applyMove(BixNix::Move(49, 41));
    b = b.applyMove(BixNix::Move(17, 25));
    b = b.applyMove(BixNix::Move(41, 33));

    std::cout << a.getHash() << std::endl;
    std::cout << b.getHash() << std::endl;
*/  


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
