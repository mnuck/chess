#include <algorithm>
#include <chrono>
#include <iomanip>
#include <vector>

#include "Engine.h"

namespace BixNix
{

Engine::Engine():
    _ponderer(nullptr),
    _ponderer_best_move(Move(0,0))
{
    
}

Engine::~Engine()
{
        
}

void Engine::init(Color color)
{
    _heuristic_runs = 0;
    _start_time = std::chrono::system_clock::now();

    if (_ponderer == nullptr)
    {
        srand(time(NULL));

        _board = Board::initial();
        _color = color;

        _ponderer_done = false;
        _ponderer_needs_new_board = true;
        _ponderer_best_move_ready = false;
        _ponderer = new std::thread(&Engine::ponder, this);
        
    }
}

void Engine::end() 
{
    auto end_time = std::chrono::system_clock::now();
    auto diff = 
        std::chrono::duration_cast<std::chrono::seconds>(end_time - _start_time);

    std::cout << _heuristic_runs << " heuristic runs" << std::endl;
    std::cout << diff.count() << " seconds" << std::endl;
    std::cout << _heuristic_runs / diff.count() << " runs per sec" << std::endl;

    _ponderer_done = true;
    if (_ponderer != nullptr)
    {
        if (_ponderer->joinable())
        {
            _ponderer->join();
        }
        delete _ponderer;
        _ponderer = nullptr;
    }
}

void Engine::ponder()
{
    Board ponderBoard;
    while (true)
    {
        if (_ponderer_done)
            return;
        
        if (_ponderer_needs_new_board)
        {
            ponderBoard = _board;
            _ponderer_needs_new_board = false;
            std::cout << "\nponderer has a new board " << std::endl;
        }
        
        std::vector<Move> potentialMoves = ponderBoard.getMoves(_color);
        trimTrifoldRepetition(ponderBoard, potentialMoves);
        _ponderer_best_move = potentialMoves[rand() % potentialMoves.size()]; // failsafe
        _ponderer_best_move_ready = true;

        float maxScore;
        unsigned int depthLimit = 1;
        while (true)
        {
            if (_ponderer_done)
                return;
            if (_ponderer_needs_new_board)
                break;
            
            maxScore = -100000;
            for (Move& m: potentialMoves)
            {
                Board b = _board.applyMove(m);
                m.score = minimax(b, _color, depthLimit);
                if (m.score > maxScore)
                    maxScore = m.score;
                if (_ponderer_done)
                    return;
                if (_ponderer_needs_new_board)
                    break;
            }
            if (_ponderer_needs_new_board)
                break;
            
            std::sort(potentialMoves.begin(), potentialMoves.end(),
                      [&](const Move& a, const Move& b) -> bool
                      { return a.score > b.score; });

            std::vector<Move> bestMoves;
            for (Move& m: potentialMoves)
            {
                if (maxScore == m.score)
                {
                    bestMoves.push_back(m);
                }
            }

            _ponderer_best_move = bestMoves[rand() % bestMoves.size()];
            _ponderer_best_move_ready = true;

            std::cout << "level " << depthLimit
                      << " best move (" << _ponderer_best_move.score
                      << "): " << _ponderer_best_move
                      << std::endl;
        
            ++depthLimit;
        }
    }
}


void Engine::reportTimeLeft(float time)
{
    _time = time;
}


void Engine::reportMove(Move move)
{
    _board = _board.applyMove(move);
    _ponderer_needs_new_board = true;
    _ponderer_best_move_ready = false;
}


Move Engine::getMove()
{
    int runTime = std::min((int)_time / 10, 10);
    std::this_thread::sleep_for(std::chrono::seconds(runTime));

    while (!_ponderer_best_move_ready)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

    Move move = _ponderer_best_move;
    _ponderer_best_move_ready = false;
    
    std::cout << "taking " << move << std::endl;
    _board = _board.applyMove(move);

    return move;
}


float Engine::minimax(Board& board, 
                      const Color& toMove,
                      const unsigned int depthLimit,
                      const MinimaxPlayer player,
                      const unsigned int depth,
                      float alpha,
                      float beta)
{
    if (_ponderer_done)
        return 0.0;

    if (_ponderer_needs_new_board)
    {
        _ponderer_best_move_ready = false;
        return 0.0;
    }

    if (depth >= depthLimit)
    {
        return heuristic(board);
    }

    float result;
    if (Max == player)
        result = -100000;
    else
        result =  100000;

    std::vector<Move> actions = board.getMoves(toMove);
    trimTrifoldRepetition(board, actions);

    if (actions.size() == 0)
    { // checkmate?
        return heuristic(board);
    }

    for (Move& m : actions)
    {
        Board resultBoard = board.applyMove(m);
        float value = 
            minimax(
                resultBoard,
                Color(1 - toMove),
                depthLimit,
                MinimaxPlayer(1 - player),
                depth + 1,
                alpha,
                beta);
        if (Max == player)
        {
            result = std::max(result, value);
            alpha  = std::max(alpha, result);
        } else {
            result = std::min(result, value);
            beta   = std::min(beta, result);
        }
        if (beta <= alpha)
        {
            break;
        }        
    }
    return result;    
}


float Engine::heuristic(Board& board)
{
    ++_heuristic_runs;

    if ((board._toMove == _color) && board.inCheckmate(_color))
    {
        return -100000;
    }
    if ((board._toMove == Color(1 - _color)) && board.inCheckmate(Color(1 - _color)))
    {
        return 100000;
    }
    
    float pawnValue = 1;
    float knightValue = 3;
    float bishopValue = 3;
    float rookValue = 5;
    float queenValue = 9;
    
    int pawnDiff = 
        __builtin_popcountll(board._pieces[Pawn] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Pawn] & board._colors[1 - _color]);

    int knightDiff = 
        __builtin_popcountll(board._pieces[Knight] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Knight] & board._colors[1 - _color]);

    int bishopDiff = 
        __builtin_popcountll(board._pieces[Bishop] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Bishop] & board._colors[1 - _color]);

    int rookDiff = 
        __builtin_popcountll(board._pieces[Rook] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Rook] & board._colors[1 - _color]);

    int queenDiff = 
        __builtin_popcountll(board._pieces[Queen] & board._colors[_color]) - 
        __builtin_popcountll(board._pieces[Queen] & board._colors[1 - _color]);

    return
        pawnValue * pawnDiff +
        knightValue * knightDiff +
        bishopValue * bishopDiff +
        rookValue * rookDiff +
        queenValue * queenDiff;
}

void Engine::trimTrifoldRepetition(Board& board, std::vector<Move>& actions)
{
    // trim trifold repetition moves for now
    actions.erase(
        std::remove_if(
            actions.begin(),
            actions.end(),
            [&] (Move& move) -> bool
            {
                size_t moveLength = board._moves.size();
                if (moveLength >= 7)
                {
                    if ((move == board._moves[moveLength - 4]) &&
                        (board._moves[moveLength - 1] == board._moves[moveLength - 5]) &&
                        (board._moves[moveLength - 2] == board._moves[moveLength - 6]) &&
                        (board._moves[moveLength - 3] == board._moves[moveLength - 7]))
                    {
                        return true;
                    }
                }
                return false;
            }),
        actions.end());    
}


}
