// cards/CardHands.cpp

#include "cards/CardHands.hpp"
#include "prim/range.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::cards {

CardHands::CardHands()
{
    // hands empty, full capacity
    mCapacities.fill(13);
}

auto CardHands::at(int i) const -> CardSet { return mHands.at(i); }

auto CardHands::operator[](int i) const -> CardSet { return mHands.at(i); }

auto CardHands::availableCapacity(int i) const -> Size_t { return mCapacities[i] - mHands.at(i).size(); }

auto CardHands::totalCapacity() const -> Size_t
{
    Size_t total = 0;
    for (auto p : prim::range(kNumPlayers))
        total += availableCapacity(p);
    return total;
}

auto CardHands::playersWithAvailableCapacity(Size_t expectedCapacity) const -> unsigned
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
    (void)capacity; // silence warning in release build
    (void)expectedCapacity; // silence warning in release build
    assert(numPlayers >= 1);
    return numPlayers;
}

auto CardHands::addCard(unsigned p, Card card) -> void
{
    assert(!mHands.at(p).hasCard(card));
    mHands.at(p) += card;
}

auto CardHands::setUnion(unsigned p, CardSet cards) -> void
{
    if (cards.empty())
        return;
    assert(availableCapacity(p) >= cards.size());
    mHands.at(p) += cards;
}

auto CardHands::prepCurrentPlayerForDeal(unsigned p, CardSet hand) -> void
{
    mCapacities[p] = hand.size();
    mHands.at(p) = hand;
}

auto CardHands::prepForDeal(unsigned p, Size_t capacity, CardSet extra) -> void
{
    mCapacities[p] = capacity;
    mHands.at(p) = extra;
}

auto CardHands::reduceAvailableCapacityTo(unsigned p, Size_t capacity) -> Size_t
{
    // This method is like prepForDeal, but used in a more specialized situation.
    // See TwoOpponentsGetSuit. We're distributing cards from one suit into this and another hand,
    // but will use DealUnknownsToHands to do the work. We need to temporarily reduce the capacity
    // to just the number of cards from the suit that we want to distribute to the hand.
    // After we're done, we need to restore the capacity to its previous value using restoreCapacityTo().
    assert(capacity <= availableCapacity(p));
    Size_t prevCapacity = mCapacities[p];
    mCapacities[p] = mHands.at(p).size() + capacity;
    return prevCapacity;
}

auto CardHands::restoreCapacityTo(unsigned p, Size_t capacity) -> void
{
    assert(mCapacities[p] == mHands.at(p).size());
    assert(capacity >= mCapacities[p]);
    mCapacities[p] = capacity;
}

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(CardHands)
{
    class_<CardHands>("CardHands").constructor<>().function("get", &CardHands::get).function("at", &CardHands::at);
}
#endif

} // namespace pho::cards
