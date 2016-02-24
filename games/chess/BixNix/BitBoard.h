#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include <cstdint>
#include <iostream>

#include "Enums.h"

namespace BixNix
{

std::string RenderBB(BitBoard bb);

std::string RenderBB(BitBoard bb);

const BitBoard notAFile  = 0x7F7F7F7F7F7F7F7F;
const BitBoard notABFile = 0x3F3F3F3F3F3F3F3F;
const BitBoard notHFile  = 0xFEFEFEFEFEFEFEFE;
const BitBoard notGHFile = 0xFCFCFCFCFCFCFCFC;

// Kogge-Stone routines from 
// https://chessprogramming.wikispaces.com/Kogge-Stone+Algorithm

BitBoard smearN(BitBoard generator, BitBoard clear);
BitBoard smearNE(BitBoard generator, BitBoard clear);
BitBoard smearE(BitBoard generator, BitBoard clear);
BitBoard smearSE(BitBoard generator, BitBoard clear);
BitBoard smearS(BitBoard generator, BitBoard clear);
BitBoard smearSW(BitBoard generator, BitBoard clear);
BitBoard smearW(BitBoard generator, BitBoard clear);
BitBoard smearNW(BitBoard generator, BitBoard clear);

BitBoard shiftN(BitBoard source);
BitBoard shiftNE(BitBoard source);
BitBoard shiftE(BitBoard source);
BitBoard shiftSE(BitBoard source);
BitBoard shiftS(BitBoard source);
BitBoard shiftSW(BitBoard source);
BitBoard shiftW(BitBoard source);
BitBoard shiftNW(BitBoard source);

}

#endif // __BITBOARD_H__
