#pragma once

#include "cards/CardSet.hpp"

namespace pho::cards
{

class FourHands
{
public:
    using Rep = std::array<CardSet, kNumPlayers>;

    ~FourHands() = default;
    FourHands() = default;
    FourHands(Rep rep) : mRep{rep} {};
    FourHands(const FourHands&) = default;
    FourHands(FourHands&&) = default;

    FourHands& operator=(const FourHands&) = default;
    FourHands& operator=(FourHands&&) = default;

    auto hand(int i) const -> CardSet;

    auto at(int i) const -> CardSet { return mRep.at(i); }
    auto at(int i) -> CardSet& { return mRep.at(i); }

private:
    Rep mRep;
};

} // namespace pho::cards
