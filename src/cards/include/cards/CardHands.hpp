// lib/CardHands.h

#pragma once

#include "cards/CardSet.hpp"
#include "prim/range.hpp"

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

    using FourHands = std::array<CardSet, 4>;

    CardHands()
    {
        // hands empty, full capacity
        mCapacities.fill(13);
    }

    CardHands(const CardHands& other) = default;
    CardHands& operator=(const CardHands& other) = delete;

    const FourHands& get() const { return mHands; }

    const CardSet& operator[](int i) const { return mHands[i]; }

    Size_t availableCapacity(int i) const { return mCapacities[i] - mHands[i].size(); }

    // TODO move most methods into .cpp file
    Size_t totalCapacity() const
    {
        Size_t total = 0;
        for (auto p : prim::range(kNumPlayers))
            total += availableCapacity(p);
        return total;
    }

    unsigned playersWithAvailableCapacity(Size_t expectedCapacity) const
    {
        unsigned numPlayers = 0;
        Size_t capacity = 0;
        for (auto p : prim::range(kNumPlayers))
        {
            Size_t cap = availableCapacity(p);
            capacity += cap;
            if (cap > 0)
                ++numPlayers;
        }
        assert(capacity == expectedCapacity);
        (void) capacity; // silence warning in release build
        assert(numPlayers >= 1);
        return numPlayers;
    }

    void addCard(unsigned p, Card card)
    {
        assert(!mHands[p].hasCard(card));
        mHands[p] += card;
    }

    void setUnion(unsigned p, const CardSet& cards)
    {
        if (cards.empty())
            return;
        assert(availableCapacity(p) >= cards.size());
        mHands[p] += cards;
    }

    void prepCurrentPlayerForDeal(unsigned p, const CardSet& hand)
    {
        mCapacities[p] = hand.size();
        mHands[p] = hand;
    }

    void prepForDeal(unsigned p, Size_t capacity, const CardSet& extra)
    {
        mCapacities[p] = capacity;
        mHands[p] = extra;
    }

    Size_t reduceAvailableCapacityTo(unsigned p, Size_t capacity)
    {
        // This method is like prepForDeal, but used in a more specialized situation.
        // See TwoOpponentsGetSuit. We're distributing cards from one suit into this and another hand,
        // but will use DealUnknownsToHands to do the work. We need to temporarily reduce the capacity
        // to just the number of cards from the suit that we want to distribute to the hand.
        // After we're done, we need to restore the capacity to its previous value using restoreCapacityTo().
        assert(capacity <= availableCapacity(p));
        Size_t prevCapacity = mCapacities[p];
        mCapacities[p] = mHands[p].size() + capacity;
        return prevCapacity;
    }

    void restoreCapacityTo(unsigned p, Size_t capacity)
    {
        assert(mCapacities[p] == mHands[p].size());
        assert(capacity >= mCapacities[p]);
        mCapacities[p] = capacity;
    }

private:
    using Capacities = std::array<Size_t, 4>;

    FourHands mHands;
    Capacities mCapacities;
};

} // namespace pho::cards
