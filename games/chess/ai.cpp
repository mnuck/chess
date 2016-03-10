// This is where you build your AI for the Chess game.

#include "ai.h"

#include "BixNix/Book.h"
#include "BixNix/Engine.h"
#include "BixNix/Logger.h"

std::string Chess::AI::getName() {
  std::string date(__DATE__);
  std::string time(__TIME__);
  if (getenv("DT_NAME"))
    return "LexMaxBixNix " + date + " " + time;
  else
    return "LexMaxBixNix";
}

void Chess::AI::start() {
  BixNix::Color myColor;
  if (player->color == "White")
    myColor = BixNix::White;
  else
    myColor = BixNix::Black;

  // SIG-Game time unit is nanoseconds, BixNix is seconds
  float myTime = player->timeRemaining / 1000000000;

  _engine.init(myColor, myTime);

  char* bookFilename;
  if (bookFilename = getenv("BOOK"))
    _book.init(std::string(bookFilename));
  else
    _book.init("book.bin");
}

void Chess::AI::gameUpdated() {}

void Chess::AI::ended(bool won, std::string reason) {
  if (getenv("REPORT")) {
    int fd = open("results.log", O_APPEND | O_WRONLY);
    std::string message = getName() + ",";
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

bool Chess::AI::runTurn() {
  if (game->moves.size() > 0) {
    BixNix::Move move = siggame2bixnix(game->moves[game->currentTurn - 1]);
    LOG(trace) << "receiving " << move << " "
               << player->timeRemaining / 1000000000 << "s remaining";

    _book.reportMove(move);
    _engine.reportMove(move, player->timeRemaining / 1000000000);
  }

  BixNix::Move response = _book.getMove();
  if (BixNix::Move(0) == response)
    response = _engine.getMove();
  else
    _engine.reportMove(response, player->timeRemaining / 1000000000);

  sendBixNixMove(response);
  return true;
}

BixNix::Move Chess::AI::siggame2bixnix(Chess::Move* m) {
  const int sourceIndex = 8 * m->fromRank - (1 + (m->fromFile)[0] - 'a');
  const int targetIndex = 8 * m->toRank - (1 + (m->toFile)[0] - 'a');

  BixNix::Piece promoteType;
  if (m->promotion == "Knight") {
    promoteType = BixNix::Knight;
  } else if (m->promotion == "Rook") {
    promoteType = BixNix::Rook;
  } else if (m->promotion == "Bishop") {
    promoteType = BixNix::Bishop;
  } else {
    promoteType = BixNix::Queen;
  }

  return BixNix::Move(sourceIndex, targetIndex, promoteType);
}

void Chess::AI::sendBixNixMove(const BixNix::Move& m) {
  const int sourceRank = 1 + (m.getSource() / 8);
  const int targetRank = 1 + (m.getTarget() / 8);

  const std::string sourceFile(1, 'a' + (7 - (m.getSource() % 8)));
  const std::string targetFile(1, 'a' + (7 - (m.getTarget() % 8)));

  for (auto* p : game->pieces) {
    if (p->file == sourceFile && p->rank == sourceRank) {
      if (m.getPromoting()) {
        std::string promoteType;
        switch (m.getPromotionPiece()) {
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
        p->move(targetFile, targetRank, promoteType);
      } else {
        p->move(targetFile, targetRank);
      }
      break;
    }
  }
}
