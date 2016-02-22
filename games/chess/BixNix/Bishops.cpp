#include "Bishops.h"

namespace BixNix
{

Bishops& Bishops::GetInstance()
{
    static Bishops instance;
    return instance;
}


BitBoard Bishops::getAttacksFrom(BitBoard bishops,
                                 BitBoard targets,
                                 BitBoard friendlies)
{
    BitBoard result(0LL);
    while (0LL != bishops)
    {
        const Square source(__builtin_ffsll(bishops) - 1);
        bishops &= ~(1LL << source);
        result |= getAttacksFrom(source, targets, friendlies);
    }
    return result;
}


BitBoard Bishops::getAttacksFrom(Square square,
                                 BitBoard targets,
                                 BitBoard friendlies)
{
    BitBoard blockers((targets | friendlies) & _moveMask[square]);
    unsigned int index((blockers * _magicNumber[square]) >> _magicShift[square]);
    BitBoard attacks(*(_magicAttacks[square] + index));
    return attacks & ~friendlies;
}


Bishops::~Bishops()
{
    if (nullptr != _data)
    {
        delete [] _data;
        _data = nullptr;
    }
}


Bishops::Bishops():
    _moveMask({
                0x40201008040200LL,
                0x402010080400LL,
                0x4020100a00LL,
                0x40221400LL,
                0x2442800LL,
                0x204085000LL,
                0x20408102000LL,
                0x2040810204000LL,
                0x20100804020000LL,
                0x40201008040000LL,
                0x4020100a0000LL,
                0x4022140000LL,
                0x244280000LL,
                0x20408500000LL,
                0x2040810200000LL,
                0x4081020400000LL,
                0x10080402000200LL,
                0x20100804000400LL,
                0x4020100a000a00LL,
                0x402214001400LL,
                0x24428002800LL,
                0x2040850005000LL,
                0x4081020002000LL,
                0x8102040004000LL,
                0x8040200020400LL,
                0x10080400040800LL,
                0x20100a000a1000LL,
                0x40221400142200LL,
                0x2442800284400LL,
                0x4085000500800LL,
                0x8102000201000LL,
                0x10204000402000LL,
                0x4020002040800LL,
                0x8040004081000LL,
                0x100a000a102000LL,
                0x22140014224000LL,
                0x44280028440200LL,
                0x8500050080400LL,
                0x10200020100800LL,
                0x20400040201000LL,
                0x2000204081000LL,
                0x4000408102000LL,
                0xa000a10204000LL,
                0x14001422400000LL,
                0x28002844020000LL,
                0x50005008040200LL,
                0x20002010080400LL,
                0x40004020100800LL,
                0x20408102000LL,
                0x40810204000LL,
                0xa1020400000LL,
                0x142240000000LL,
                0x284402000000LL,
                0x500804020000LL,
                0x201008040200LL,
                0x402010080400LL,
                0x2040810204000LL,
                0x4081020400000LL,
                0xa102040000000LL,
                0x14224000000000LL,
                0x28440200000000LL,
                0x50080402000000LL,
                0x20100804020000LL,
                0x40201008040200LL
        }),
    _magicShift({
                58,59,59,59,59,59,59,58,
                59,59,59,59,59,59,59,59,
                59,59,57,57,57,57,59,59,
                59,59,57,55,55,57,59,59,
                59,59,57,55,55,57,59,59,
                59,59,57,57,57,57,59,59,
                59,59,59,59,59,59,59,59,
                58,59,59,59,59,59,59,58}),
    _magicNumber({
                0x450010148020840LL,
                0x10048100420000LL,
                0x4010401080008LL,
                0x9040300400004LL,
                0x2408484010000004LL,
                0x1112010480100LL,
                0x400420211410800LL,
                0x808400a00400LL,
                0x5001100410040050LL,
                0x13820a1094128484LL,
                0x208100086085000LL,
                0x1044080849000830LL,
                0x2020020210014040LL,
                0xc210008804402801LL,
                0x21008208024082LL,
                0x620124010440LL,
                0x40042008421081LL,
                0x10292084288091LL,
                0x20180990044010a1LL,
                0x612002422020200LL,
                0xa004422011021LL,
                0x4025001201010168LL,
                0x100200bc01410828LL,
                0xa280206011408LL,
                0x4282080020881010LL,
                0x11040200a0a12LL,
                0x1008880210044811LL,
                0x4201080061004100LL,
                0x8045011001014001LL,
                0x25040100c4900090LL,
                0x1041840440402LL,
                0x10404240101090cLL,
                0xa101268402e80LL,
                0x8008a82800610200LL,
                0x3081004808810800LL,
                0x2004040140101LL,
                0x1001010400020020LL,
                0x100808300020120LL,
                0x1830020608006100LL,
                0x80120c2620510080LL,
                0x80013010ec401010LL,
                0x11043002004404LL,
                0xa001001802000404LL,
                0x4420216000400LL,
                0x402200a002500LL,
                0x404010010040420aLL,
                0x8100106088440LL,
                0x2041402310080LL,
                0x614040213300000LL,
                0x40809404092c0840LL,
                0x680002020092220eLL,
                0x80020020881200LL,
                0x380041002022005LL,
                0x2080328220100LL,
                0xa028883004007010LL,
                0x40020802248a0000LL,
                0x245004802480200LL,
                0x1102604108288202LL,
                0xc24020902882400LL,
                0x2080080941400LL,
                0x12080011020200LL,
                0x80040820200d3241LL,
                0x1502410108a00LL,
                0x1020421008408481LL}),
    _data(nullptr)
{
    _data = new BitBoard[5248];

    size_t squareIndex = 0;

    for (Square square = 0 ; square < 64 ; ++square)
    {
        _magicAttacks[square] = _data + squareIndex;
        squareIndex += (1 << (64 - _magicShift[square]));

        for (BitBoard occupied : genOccupancyVariations(square))
        {
            BitBoard bishops(1LL << square);

            BitBoard neAttacks = shiftNE(smearNE(bishops, ~occupied));
            BitBoard seAttacks = shiftSE(smearSE(bishops, ~occupied));
            BitBoard swAttacks = shiftSW(smearSW(bishops, ~occupied));
            BitBoard nwAttacks = shiftNW(smearNW(bishops, ~occupied));
            BitBoard attacks = 
                neAttacks | seAttacks | swAttacks | nwAttacks;

            BitBoard blockers(occupied & _moveMask[square]);
            unsigned int index((blockers * _magicNumber[square]) >> _magicShift[square]);
            *(_magicAttacks[square] + index) = attacks;
        }
    }
}


std::vector<BitBoard> Bishops::genOccupancyVariations(Square square)
{
    std::vector<BitBoard> result;
    BitBoard variationCount(1LL << __builtin_popcountll(_moveMask[square]));
    std::vector<size_t> indexOfOnesInMask;
    for (size_t j = 0; j < 64; ++j)
        if (_moveMask[square] & (1LL << j))
            indexOfOnesInMask.push_back(j);

    for (BitBoard i = 0; i < variationCount; ++i)
    {
        BitBoard variation(0LL);

        for (size_t j = 0; j < indexOfOnesInMask.size(); ++j)
            if (i & (1LL << j))
                variation |= (1LL << indexOfOnesInMask[j]);

        result.push_back(variation);
    }

    return result;
}


}
