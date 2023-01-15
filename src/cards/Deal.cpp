#include <assert.h>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "cards/Deal.hpp"
#include "math/combinatorics.hpp"
#include "math/random.hpp"
#include "prim/range.hpp"

namespace pho::cards {

using namespace std;

using uint128_t = math::uint128_t;

static uint128_t kPossibleDistinguishableDeals = math::possibleDistinguishableDeals();

int Deal::startPlayer() const
{
    // The 2 clubs is card number 0
    // The hands are always sorted, so we only need to look at first card of each hand
    for (auto p : prim::range(kNumPlayers))
    {
        if (PeekAt(p, 0) == 0)
            return p;
    }
    assert(false);
    return -1;
}

Deal::Deal()
: mDealIndex(randomDealIndex())
{
    DealHands(mDealIndex);
}

Deal::Deal(uint128_t index)
: mDealIndex(index)
{
    DealHands(mDealIndex);
}

Deal::Deal(const std::string& dealIndex)
: Deal(dealIndex.empty() ? randomDealIndex() : math::parseHex128(dealIndex.c_str()))
{ }

uint128_t Deal::randomDealIndex(const RandomGenerator& rng) { return rng.range128(kPossibleDistinguishableDeals); }

uint128_t Deal::randomDealIndex() { return randomDealIndex(RandomGenerator::ThreadSpecific()); }

void Deal::DealHands(uint128_t I) { DealUnknownsToHands(CardSet::fullDeck(), mHands, I); }

#ifndef NDEBUG
void validateDealUnknowns(CardSet unknowns, const CardHands& hands)
{
    unsigned T = 0;
    for (auto p : prim::range(kNumPlayers))
    {
        unsigned avail = hands.availableCapacity(p);
        assert(avail <= kCardsPerHand);
        T += avail;
    }
    assert(T == unknowns.size());
}
#endif

uint128_t possibleDealsUnknownsToHands(CardSet unknowns, const CardHands& hands)
{
#ifndef NDEBUG
    validateDealUnknowns(unknowns, hands);
#endif

    uint128_t result = 1;

    unsigned D = unknowns.size();
    for (auto p : prim::range(kNumPlayers))
    {
        unsigned H = hands.availableCapacity(p);
        assert(H <= D);
        result *= math::combinations128(D, H);
        D -= H;
    }
    assert(D == 0);
    return result;
}

void DealUnknownsToHands(CardSet unknowns, CardHands& hands)
{
    DealUnknownsToHands(unknowns, hands, Deal::randomDealIndex());
}

void DealUnknownsToHands(CardSet unknowns, CardHands& hands, uint128_t index)
{
    // This code is adapted from http://www.rpbridge.net/7z68.htm
    const uint128_t kPossibleDeals = possibleDealsUnknownsToHands(unknowns, hands);
    assert(index < kPossibleDeals);

    auto it = unknowns.begin();
    uint128_t K = kPossibleDeals;
    for (unsigned C = unknowns.size(); C > 0; --C)
    {
        uint128_t X = 0;
        for (auto p : prim::range(kNumPlayers))
        {
            index -= X;
            X = (K * hands.availableCapacity(p)) / C;
            if (index < X)
            {
                hands.addCard(p, *it);
                ++it;
                break;
            }
        }
        K = X;
    }
}

void Deal::printDeal() const
{
    for (auto p : prim::range(kNumPlayers))
    {
        for (auto i : prim::range(kCardsPerHand))
            fmt::print("{} ", nameOfCard(PeekAt(p, i)));
        fmt::print("\n");
    }
    fmt::print("\n");
}

CardSet Deal::dealFor(int player) const { return mHands[player]; }

namespace {
Card removeACardAtRandom_(CardSet& hand, const math::RandomGenerator& rng)
{
    unsigned choice = unsigned(rng.range64(hand.size()));
    auto card = hand.nthCard(choice);
    hand -= card;
    return card;
}
} // namespace

// This is a special constructor that can be used by a PassThree algorithm.
Deal::Deal(CardSet carlosHand, const RandomGenerator& rng)
: mDealIndex(~uint128_t{0})
{
    assert(carlosHand.size() == kCardsPerHand);

    CardSet others{CardSet::kAllCards};
    others -= carlosHand;
    assert(others.size() == kCardsPerDeck - kCardsPerHand);

    assert(mHands.totalCapacity() == kCardsPerDeck);
    mHands.setUnion(0, carlosHand);
    assert(mHands.totalCapacity() == kCardsPerDeck - kCardsPerHand);

    for (auto i : prim::range(kCardsPerHand))
    {
        (void) i;
        mHands.addCard(1, removeACardAtRandom_(others, rng));
        mHands.addCard(2, removeACardAtRandom_(others, rng));
    }
    assert(others.size() == kCardsPerHand);

    assert(mHands.availableCapacity(3) == kCardsPerHand);
    mHands.setUnion(3, others);
    assert(mHands.totalCapacity() == 0);
}

} // namespace pho::cards
