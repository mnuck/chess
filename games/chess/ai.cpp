// This is where you build your AI for the Chess game.

#include "ai.h"

#include "BixNix/Engine.h"


std::string Chess::AI::getName()
{
    return "LexMaxBixNix";
}

void Chess::AI::start()
{
    BixNix::Color myColor;
    if (player->color == "White")
        myColor = BixNix::White;
    else
        myColor = BixNix::Black;

    // SIG-Game time unit is nanoseconds, BixNix is seconds
    float myTime = player->timeRemaining / 1000000000;

    _engine.init(myColor, myTime);
}

void Chess::AI::gameUpdated() {}

void Chess::AI::ended(bool won, std::string reason)
{
    _engine.end();
}

bool Chess::AI::runTurn()
{
    if (game->moves.size() > 0)
        _engine.reportMove(
            siggame2bixnix(game->moves[game->currentTurn]),
            player->timeRemaining / 1000000000);
    sendBixNixMove(_engine.getMove());
    return true;
}


BixNix::Move Chess::AI::siggame2bixnix(const std::string& move)
{
    // ignore the move string, analyze the piece list
    SELF EMBARGO VIA FAILURE TO COMPILE
    
    // return a dummy for now
    return BixNix::Move(0, 0, BixNix::Queen);
}

void Chess::AI::sendBixNixMove(const BixNix::Move& m)
{
    const int sourceFile = 1 + (m.getSource() / 8);
    const int targetFile = 1 + (m.getTarget() / 8);

    const std::string sourceRank(1, 'a' + (7 - (m.getSource() % 8)));
    const std::string targetRank(1, 'a' + (7 - (m.getTarget() % 8)));

    for (auto* p : game->pieces)
    {
        if (p->file == sourceFile && p->rank == sourceRank)
        {
            if (m.getPromoting())
            {
                std::string promoteType;
                switch (m.getPromotionPiece())
                {
                case BixNix::Knight:
                    promoteType = "Knight";
                    break;
                case BixNix::Rook:
                    promoteType = "Rook";
                    break;
                case BixNix::Bishop:
                    promoteType = "Bishop";
                    break;
                case BixNix::Queen:
                default:
                    promoteType = "Queen";
                    break;
                }
                p->move(targetRank, targetFile, promoteType);
            }
            else
            {
                p->move(targetRank, targetFile);
            }
            break;
        }
    }
}
