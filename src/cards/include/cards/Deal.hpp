#pragma once

#include <set>

#include "cards/Card.hpp"
#include "cards/CardHands.hpp"
#include "cards/CardSet.hpp"
#include "math/math.hpp"
#include "math/random.hpp"

namespace pho::cards {

using DealIndex = math::uint128_t;

DealIndex possibleDealsUnknownsToHands(CardSet unknowns, const CardHands& hands);
void DealUnknownsToHands(CardSet unknowns, CardHands& hands);
void DealUnknownsToHands(CardSet unknowns, CardHands& hands, DealIndex index);
void validateDealUnknowns(CardSet unknowns, const CardHands& hands);

class Deal
{
public:
    using RandomGenerator = math::RandomGenerator;

    Deal();
    // Creates a random deal

    Deal(DealIndex index);
    // Creates a deal for the given index.

    Deal(const std::string& dealIndex);
    // If dealIndex is the empty string, create a random deal
    // Otherwise parse the hex string into the deal index to use.

    DealIndex dealIndex() const { return mDealIndex; }

    void printDeal() const;

    CardSet dealFor(int player) const;
    // player is a number 0..3
    // Returns a vector of the 13 cards dealt to the player

    int startPlayer() const;
    // Return number of player who has the 2 clubs.

    static DealIndex randomDealIndex(const RandomGenerator& rng);
    static DealIndex randomDealIndex();
    // Generate a random number in the range [0, 52!/(13!^4))

    Card PeekAt(int p, int c) const { return mHands[p].nthCard(c); }
    Card PeekAt(int i) const { return PeekAt(i / 13, i % 13); }
    // For unit tests, peak at the card at a given card location

    // This is a special constructor that can be used by a PassThree algorithm.
    // We just need a hypothetical deal where player 0 ("Carlo") has the given hand,
    // and the other three players are given a random arrangement of the other cards.
    Deal(CardSet carlosHand, const RandomGenerator& rng);

private:
    /// @brief Given a deal index, create the unique Deal for that index
    /// @param index An large unsigned (128bit) integer less than 52!/(13!^4)
    void DealHands(DealIndex index);

private:
    const DealIndex mDealIndex;
    CardHands mHands;
};

}  // namespace pho::cards
