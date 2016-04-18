#include "BitBoard.h"
#include "Board.h"
#include "Rooks.h"

namespace BixNix {

Rooks& Rooks::GetInstance() {
  static Rooks instance;
  return instance;
}

BitBoard Rooks::getAttacksFrom(BitBoard rooks, BitBoard targets,
                               BitBoard friendlies) {
  BitBoard result(0LL);
  while (0LL != rooks) {
    const Square source(__builtin_ffsll(rooks) - 1);
    rooks &= ~(1LL << source);
    result |= getAttacksFrom(source, targets, friendlies);
  }
  return result;
}

BitBoard Rooks::getAttacksFrom(Square square, BitBoard targets,
                               BitBoard friendlies) {
  BitBoard blockers((targets | friendlies) & _moveMask[square]);
  unsigned int index((blockers * _magicNumber[square]) >> _magicShift[square]);
  BitBoard attacks(*(_magicAttacks[square] + index));
  return attacks & ~friendlies;
}

Rooks::~Rooks() {
  if (nullptr != _data) {
    delete[] _data;
    _data = nullptr;
  }
}

Rooks::Rooks()
    : _moveMask(
          {0x000101010101017eLL, 0x000202020202027cLL, 0x000404040404047aLL,
           0x0008080808080876LL, 0x001010101010106eLL, 0x002020202020205eLL,
           0x004040404040403eLL, 0x008080808080807eLL, 0x0001010101017e00LL,
           0x0002020202027c00LL, 0x0004040404047a00LL, 0x0008080808087600LL,
           0x0010101010106e00LL, 0x0020202020205e00LL, 0x0040404040403e00LL,
           0x0080808080807e00LL, 0x00010101017e0100LL, 0x00020202027c0200LL,
           0x00040404047a0400LL, 0x0008080808760800LL, 0x00101010106e1000LL,
           0x00202020205e2000LL, 0x00404040403e4000LL, 0x00808080807e8000LL,
           0x000101017e010100LL, 0x000202027c020200LL, 0x000404047a040400LL,
           0x0008080876080800LL, 0x001010106e101000LL, 0x002020205e202000LL,
           0x004040403e404000LL, 0x008080807e808000LL, 0x0001017e01010100LL,
           0x0002027c02020200LL, 0x0004047a04040400LL, 0x0008087608080800LL,
           0x0010106e10101000LL, 0x0020205e20202000LL, 0x0040403e40404000LL,
           0x0080807e80808000LL, 0x00017e0101010100LL, 0x00027c0202020200LL,
           0x00047a0404040400LL, 0x0008760808080800LL, 0x00106e1010101000LL,
           0x00205e2020202000LL, 0x00403e4040404000LL, 0x00807e8080808000LL,
           0x007e010101010100LL, 0x007c020202020200LL, 0x007a040404040400LL,
           0x0076080808080800LL, 0x006e101010101000LL, 0x005e202020202000LL,
           0x003e404040404000LL, 0x007e808080808000LL, 0x7e01010101010100LL,
           0x7c02020202020200LL, 0x7a04040404040400LL, 0x7608080808080800LL,
           0x6e10101010101000LL, 0x5e20202020202000LL, 0x3e40404040404000LL,
           0x7e80808080808000LL}),
      _magicShift({52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54,
                   53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54,
                   54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54,
                   54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53,
                   53, 53, 53, 52}),
      _magicNumber(
          {0x2180004000142683LL, 0x440004010082000LL, 0x2080082000801000LL,
           0x880080004801002LL, 0x6000a0004201810LL, 0x4100010002080400LL,
           0x400040082010810LL, 0xa200108024010042LL, 0x2220800020984000LL,
           0x4060400050002000LL, 0x9001001020050840LL, 0x8041000820100100LL,
           0x445000d00500800LL, 0x3000204000900LL, 0x4081000100020004LL,
           0x6000050a20104LL, 0x402c8000884004LL, 0x410014020004013LL,
           0x2020010288041LL, 0x4a80828008001000LL, 0x8c008080040800LL,
           0x2010100040008LL, 0x5000010100040200LL, 0x40060008610084LL,
           0xc000842280004000LL, 0x400040201000LL, 0x2c20010010410aLL,
           0x801000900100020LL, 0x800040080800800LL, 0x6000404002010LL,
           0x20010400482210LL, 0x80086002c0441LL, 0x400080800020LL,
           0x510002008404004LL, 0x5020208012004200LL, 0x18801000800800LL,
           0x8004200400400LL, 0x840044008012010LL, 0xa8800200800100LL,
           0x80108402002041LL, 0x448001402014c004LL, 0x9001402000c002LL,
           0x20040200101000LL, 0x90008008028011LL, 0x30040008008080LL,
           0x1052000400808002LL, 0x48100248040081LL, 0x401104400820001LL,
           0xb2100c020800300LL, 0x100804000200880LL, 0x1801000200280LL,
           0x1018100008008080LL, 0x40a080004008080LL, 0x20080040080LL,
           0x8000020110080400LL, 0xa001804084011a00LL, 0x49002080041843LL,
           0x22a82104b1400081LL, 0x10120001041000dLL, 0x8040210010000509LL,
           0x600102408e022LL, 0x2002008110080402LL, 0x100200914804LL,
           0x100040228904102LL}),
      _data(nullptr) {
  _data = new BitBoard[102400];

  size_t squareIndex = 0;

  for (Square square = 0; square < 64; ++square) {
    _magicAttacks[square] = _data + squareIndex;
    squareIndex += (1 << (64 - _magicShift[square]));

    for (BitBoard occupied : genOccupancyVariations(square)) {
      BitBoard rooks(1LL << square);
      BitBoard northAttacks = shiftN(smearN(rooks, ~occupied));
      BitBoard southAttacks = shiftS(smearS(rooks, ~occupied));
      BitBoard eastAttacks = shiftE(smearE(rooks, ~occupied));
      BitBoard westAttacks = shiftW(smearW(rooks, ~occupied));

      BitBoard attacks =
          northAttacks | southAttacks | eastAttacks | westAttacks;

      BitBoard blockers(occupied & _moveMask[square]);
      unsigned int index((blockers * _magicNumber[square]) >>
                         _magicShift[square]);
      *(_magicAttacks[square] + index) = attacks;
    }
  }
}

std::vector<BitBoard> Rooks::genOccupancyVariations(Square square) {
  std::vector<BitBoard> result;
  BitBoard variationCount(1LL << __builtin_popcountll(_moveMask[square]));
  std::vector<size_t> indexOfOnesInMask;
  for (size_t j = 0; j < 64; ++j)
    if (_moveMask[square] & (1LL << j)) indexOfOnesInMask.push_back(j);

  for (BitBoard i = 0; i < variationCount; ++i) {
    BitBoard variation(0LL);

    for (size_t j = 0; j < indexOfOnesInMask.size(); ++j)
      if (i & (1LL << j)) variation |= (1LL << indexOfOnesInMask[j]);

    result.push_back(variation);
  }

  return result;
}
}
