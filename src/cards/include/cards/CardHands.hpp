// cards/CardHands.hpp

#pragma once

#include "cards/CardSet.hpp"
#include "cards/FourHands.hpp"

namespace pho::cards {

class CardHands
{
public:
    using Size_t = uint8_t;

    // The `capacity` of a hand is the number of cards that it should have.
    // The 'available capacity' it the capacity minus the number of cards currently assigned to the hand.

    // This capacity for each hand at a given play/state of the game is determined by the combination
    // of the play number and which player is leading the current trick.
    // The total capacity decrements for each card played (i.e. by 4 each trick).

    // When we deal a deck at the start of a game to the four hands, each hand is empty and its capacity is 13.
    // The capacity remains at 13, but as each card is added to the hand, the available capacity
    // is reduced by one.

    // The reason for tracking capacities is to enable creating a "hypothetical" gamestate.
    // There is set of unknown cards that might exist in any of the three hands,
    // but there are also constraints, e.g. if player revealed they are void in a suit
    // then any other unplayed cards in that suit can only exist in the other two players
    // hands.

    // Being able to correctly generate (in principle) every possible hypothetical state
    // is complex. The code to do it is spread over many of the classes in lib.
    // It may be worthwhile to extract these classes out of lib into their own component,
    // or otherwise reorganize/refactor the classes.

    // Capacities should be remain constant for the life of a CardHands instance.
    // Only the AvailableCapacities change, as cards are inserted into the hands.

    CardHands();

    CardHands(const CardHands&) = default;
    CardHands(CardHands&&) = default;
    CardHands& operator=(const CardHands&) = default;
    CardHands& operator=(CardHands&&) = default;

    auto get() const -> FourHands { return mHands; }

    auto operator[](int i) const -> CardSet;
    auto at(int i) const -> CardSet;

    auto availableCapacity(int i) const -> Size_t;

    auto totalCapacity() const -> Size_t;

    auto playersWithAvailableCapacity(Size_t expectedCapacity) const -> unsigned;

    auto addCard(unsigned p, Card card) -> void;

    auto setUnion(unsigned p, CardSet cards) -> void;

    auto prepCurrentPlayerForDeal(unsigned p, CardSet hand) -> void;

    auto prepForDeal(unsigned p, Size_t capacity, CardSet extra) -> void;

    // This method is like prepForDeal, but used in a more specialized situation.
    // See TwoOpponentsGetSuit. We're distributing cards from one suit into this and another hand,
    // but will use DealUnknownsToHands to do the work. We need to temporarily reduce the capacity
    // to just the number of cards from the suit that we want to distribute to the hand.
    // After we're done, we need to restore the capacity to its previous value using restoreCapacityTo().
    auto reduceAvailableCapacityTo(unsigned p, Size_t capacity) -> Size_t;

    auto restoreCapacityTo(unsigned p, Size_t capacity) -> void;

private:
    using Capacities = std::array<Size_t, 4>;

    FourHands mHands;
    Capacities mCapacities;
};

} // namespace pho::cards
