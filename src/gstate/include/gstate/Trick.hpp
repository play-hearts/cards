#pragma once

#include "cards/Card.hpp"
#include <cassert>

namespace pho::gstate
{
using namespace pho::cards;

using PlayerNum = uint32_t; // 0..3

class Trick
{
public:
    ~Trick() = default;

    Trick()
    : mLead{kNumPlayers}
    { clear(); }

    Trick(const Trick&) = default;
    Trick(Trick&&) = default;

    Trick& operator=(const Trick&) = default;
    Trick& operator=(Trick&&) = default;

    auto at(int i) const -> Card { return mRep.at(i); }
    auto operator[](int i) -> Card& { return mRep[i]; }

    auto begin() const { return mRep.begin(); }
    auto end() const { return mRep.end(); }

    // Return the suit chosen by the player leading this trick
    auto trickSuit() const -> Suit;

    // Return the player leading the trick
    auto lead() const -> PlayerNum { return mLead; }

    // The card that started this trick
    auto leadCard() const -> Card;

    auto resetTrick(PlayerNum lead) { clear(); assert(lead<4); mLead=lead; }

    // Return the player/seat that won the trick
    auto winner() const -> PlayerNum;

    auto highCard() const -> Card;

    // given i < 4u, return the card played at that turn in this trick
    auto getTrickPlay(unsigned i) const -> Card;

    using Rep = std::array<Card, kNumPlayers>;
    Trick(const Rep& rep, PlayerNum lead=0) : mLead{lead}, mRep{rep} {}

    auto rep() const -> const Rep { return mRep; }

private:
    auto clear() -> void { mRep.fill(Card::kNone); }

private:

    uint32_t mLead;
    Rep mRep;
};

// This is not strictly required, but it is good to know that a Trick is just four bytes.
static_assert(sizeof(Card) == 1);
static_assert(sizeof(Trick) == 8);

} // namespace pho::gstate
