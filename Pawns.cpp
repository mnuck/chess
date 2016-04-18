#include "BitBoard.h"
#include "Board.h"
#include "Pawns.h"

namespace BixNix {

Pawns& Pawns::GetInstance() {
  static Pawns instance;
  return instance;
}

Pawns::~Pawns() {
  // nada
}

Pawns::Pawns() {
  // nada
}

BitBoard Pawns::getAttacksFrom(BitBoard attackers, BitBoard targets,
                               Color color) {
  BitBoard westAttacks, eastAttacks;
  if (White == color) {
    westAttacks = (attackers & notAFile) << 9;
    eastAttacks = (attackers & notHFile) << 7;
  } else {
    westAttacks = (attackers & notAFile) >> 7;
    eastAttacks = (attackers & notHFile) >> 9;
  }
  return (eastAttacks | westAttacks) & targets;
}

BitBoard Pawns::getMovesFrom(BitBoard pawns, BitBoard blockers, Color color) {
  BitBoard clear(~blockers);
  if (White == color)
    return clear & (pawns << 8);
  else
    return clear & (pawns >> 8);
}

BitBoard Pawns::getDoublePushesFrom(BitBoard pawns, BitBoard blockers,
                                    Color color) {
  BitBoard clear(~blockers);
  if (White == color) {
    pawns &= 0x000000000000FF00LL;
    if (clear & (pawns << 8)) return clear & (pawns << 16);
  } else {
    pawns &= 0x00FF000000000000LL;
    if (clear & (pawns >> 8)) return clear & (pawns >> 16);
  }
  return 0LL;
}
}
