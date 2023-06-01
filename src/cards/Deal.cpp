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

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::cards {

using namespace std;

using uint128_t = math::uint128_t;

static uint128_t kPossibleDistinguishableDeals = math::possibleDistinguishableDeals();

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

CardSet Deal::dealFor(PlayerNum player) const { return mHands[player]; }

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
Deal::Deal(CardSet playersHand, const RandomGenerator& rng, PlayerNum player)
: mDealIndex(~uint128_t{0})
{
    assert(playersHand.size() == kCardsPerHand);
    assert(player < int(kNumPlayers));

    CardSet others{CardSet::kAllCards};
    others -= playersHand;
    assert(others.size() == kCardsPerDeck - kCardsPerHand);

    assert(mHands.totalCapacity() == kCardsPerDeck);
    mHands.setUnion(player, playersHand);
    assert(mHands.totalCapacity() == kCardsPerDeck - kCardsPerHand);

    auto o1 = (player+1) % kNumPlayers;
    auto o2 = (player+2) % kNumPlayers;
    auto o3 = (player+3) % kNumPlayers;
    for (auto i : prim::range(kCardsPerHand))
    {
        (void) i;
        mHands.addCard(o1, removeACardAtRandom_(others, rng));
        mHands.addCard(o2, removeACardAtRandom_(others, rng));
    }
    assert(others.size() == kCardsPerHand);

    assert(mHands.availableCapacity(o3) == kCardsPerHand);
    mHands.setUnion(o3, others);
    assert(mHands.totalCapacity() == 0);
}

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(Deal) {
  class_<Deal>("Deal")
    .constructor<const std::string&>()
    .function("dealFor", &Deal::dealFor)
    .function("indexAsHexString", &Deal::indexAsHexString)
    ;
}
#endif

} // namespace pho::cards
