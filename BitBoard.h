#pragma once

#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include <cstdint>
#include <iostream>

typedef uint64_t BitBoard;
typedef uint8_t Square;

std::ostream& operator<<(std::ostream& lhs, const BitBoard& rhs);

const BitBoard notAFile  = 0x7F7F7F7F7F7F7F7F;
const BitBoard notABFile = 0x3F3F3F3F3F3F3F3F;
const BitBoard notHFile  = 0xFEFEFEFEFEFEFEFE;
const BitBoard notGHFile = 0xFCFCFCFCFCFCFCFC;

#endif // __BITBOARD_H__
