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
    if (getenv("REPORT"))
    {
        int fd = open("results.log", O_APPEND | O_WRONLY);
        std::string message = "";
        if (won)
            message += "Win,";
        else
            message += "Loss,";
    
        message += player->color;
        message += ",";
        message += reason;

        message += "\n";
        int junk = write(fd, message.c_str(), message.size());
        close(fd);
    }    
    
    _engine.end();
}

bool Chess::AI::runTurn()
{
    if (game->moves.size() > 0)
        _engine.reportMove(
            siggame2bixnix(game->moves[game->currentTurn - 1]),
            player->timeRemaining / 1000000000);
    sendBixNixMove(_engine.getMove());
    return true;
}

BixNix::Move Chess::AI::siggame2bixnix(Chess::Move* m)
{
    const int sourceIndex = 8 * m->fromFile - (1 + (m->fromRank)[0] - 'a');
    const int targetIndex = 8 * m->toFile - (1 + (m->toRank)[0]   - 'a');

    BixNix::Piece promoteType;
    if (m->promotion == "Knight")
    {
        promoteType = BixNix::Knight;
    }
    else if (m->promotion == "Rook")
    {
        promoteType = BixNix::Rook;
    }
    else if (m->promotion == "Bishop")
    {
        promoteType = BixNix::Bishop;
    }
    else
    {
        promoteType = BixNix::Queen;
    }

    return BixNix::Move(sourceIndex, targetIndex, promoteType);
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
