#include "BitBoard.h"
#include "Board.h"
#include "Pawns.h"

Pawns& Pawns::GetInstance()
{
    static Pawns instance;
    return instance;
}


Pawns::~Pawns()
{
    // nada
}

Pawns::Pawns()
{
    // nada
}


BitBoard Pawns::getAttacksFrom(BitBoard attackers,
                               BitBoard targets,
                               Board::Color color)
{
	BitBoard westAttacks, eastAttacks;
	if (Board::White == color)
	{
		westAttacks = attackers & notAFile << 9;
		eastAttacks = attackers & notHFile << 7;
	}
	else
	{
		westAttacks = attackers & notAFile >> 7;
		eastAttacks = attackers & notHFile >> 9;
	}
	return (eastAttacks | westAttacks) & targets;
}


BitBoard Pawns::getMovesFrom(BitBoard pawns,
                             BitBoard blockers,
                             Board::Color color)
{
	BitBoard oneStep, twoStep, homeRow, fwdOne;
	if (Board::White == color)
	{
		oneStep = (pawns << 8) & ~blockers;

		homeRow = 0x000000000000FF00LL;
		fwdOne  = 0x0000000000FF0000LL;

		twoStep =
			~blockers &
			((fwdOne & oneStep) << 8) &
			((homeRow & pawns) << 16);
	}
	else
	{
		oneStep = (pawns >> 8) & ~blockers;

		homeRow = 0x00FF000000000000LL;
		fwdOne  = 0x0000FF0000000000LL;

		twoStep =
			~blockers &
			((fwdOne & oneStep) >> 8) &
			((homeRow & pawns) >> 16);
	}

    return oneStep | twoStep;
}
