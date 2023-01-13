// cards/Card.h

#pragma once

#include "cards/constants.hpp"

#include <array>
#include <string>

namespace pho::cards {

using Nib = std::int8_t; // We only need 2 bits, but we use 8. For representing Rank and Suit.
using Ord = std::uint8_t; // We only need 6 bits, but we use 8. For values 0..51.

// We represent one card as an unsigned integer in the range 0..51
using Card = Ord;

constexpr Card kNoCard{63};

enum Suit : Nib
{
    kClubs = 0u,
    kDiamonds,
    kSpades,
    kHearts
};

constexpr auto allSuits = std::array<Suit, 4>{kClubs, kDiamonds, kSpades, kHearts};

enum Rank : Nib
{
    kTwo = 0u,
    kThree,
    kFour,
    kFive,
    kSix,
    kSeven,
    kEight,
    kNine,
    kTen,
    kJack,
    kQueen,
    kKing,
    kAce,
};

constexpr Suit suitOf(Card card) { return Suit(card / kCardsPerSuit); }
constexpr Rank rankOf(Card card) { return Rank(card % kCardsPerSuit); }
constexpr Card cardFor(Rank rank, Suit suit) { return suit * kCardsPerSuit + rank; }

constexpr Nib iSuitOf(Card card) { return Nib(suitOf(card)); }
constexpr Nib iRankOf(Card card) { return Nib(rankOf(card)); }

// More convenient for range loop over cards
constexpr Card kNumCards = Card(kCardsPerDeck);

std::string nameOfSuit(Suit suit);
std::string nameOfCard(Card card);

using Trick = std::array<Card, kNumPlayers>;

} // namespace pho::cards
