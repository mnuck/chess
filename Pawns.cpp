#include "BitBoard.h"
#include "Board.h"
#include "Pawns.h"

namespace BixNix
{

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
                               Color color)
{
	BitBoard westAttacks, eastAttacks;
	if (White == color)
	{
		westAttacks = (attackers & notAFile) << 9;
		eastAttacks = (attackers & notHFile) << 7;
	}
	else
	{
		westAttacks = (attackers & notAFile) >> 7;
        eastAttacks = (attackers & notHFile) >> 9;
	}
	return (eastAttacks | westAttacks) & targets;
}


BitBoard Pawns::getMovesFrom(BitBoard pawns,
                             BitBoard blockers,
                             Color color)
{
	BitBoard oneStep, twoStep, homeRow, fwdOne;
	if (White == color)
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

}
