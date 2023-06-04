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

    using PlayerNum = unsigned;

    // Creates a random deal
    Deal();

    // Creates a deal for the given index.
    Deal(DealIndex index);

    // If dealIndex is the empty string, create a random deal
    // Otherwise parse the hex string into the deal index to use.
    Deal(const std::string& dealIndex);

    DealIndex dealIndex() const { return mDealIndex; }

    std::string indexAsHexString() const { return math::asHexString(mDealIndex, 25u); }

    void printDeal() const;

    // player is a number 0..3
    // Returns a vector of the 13 cards dealt to the player
    CardSet dealFor(PlayerNum player) const;

    auto hands() const -> FourHands { return mHands.get(); }

static DealIndex randomDealIndex(const RandomGenerator& rng);
    static DealIndex randomDealIndex();
    // Generate a random number in the range [0, 52!/(13!^4))

    Card PeekAt(PlayerNum p, int c) const { return mHands[p].nthCard(c); }
    Card PeekAt(int i) const { return PeekAt(i / 13, i % 13); }
    // For unit tests, peak at the card at a given card location

    Deal(CardSet playersHand, const RandomGenerator& rng, PlayerNum player=0);
    // Create a deal where the given player has the given hand.
private:
    /// @brief Given a deal index, create the unique Deal for that index
    /// @param index An large unsigned (128bit) integer less than 52!/(13!^4)
    void DealHands(DealIndex index);

private:
    const DealIndex mDealIndex;
    CardHands mHands;
};

} // namespace pho::cards
