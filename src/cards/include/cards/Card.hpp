// cards/Card.h

#pragma once

#include "cards/constants.hpp"

#include <array>
#include <string>

namespace pho::cards {

using Nib = std::int8_t; // We only need 2 bits, but we use 8. For representing Rank and Suit.
using Ord = std::uint8_t; // We only need 6 bits, but we use 8. For values 0..51.

enum class Suit : Nib
{
    kClubs = 0,
    kDiamonds,
    kSpades,
    kHearts,

    kUnknown
};

constexpr auto allSuits = std::array<Suit, 4>{Suit::kClubs, Suit::kDiamonds, Suit::kSpades, Suit::kHearts};

enum class Rank : Nib
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

class Card
{
public:
    static constexpr auto kNone = Ord{63};

    constexpr Card() : card{kNone} {}
    constexpr Card(Ord c) : card{c} {}

    Card(const Card&) = default;
    Card(Card&&) = default;
    Card& operator=(const Card&) = default;
    Card& operator=(Card&&) = default;

    constexpr Ord ord() const { return card; }
    constexpr Suit suit() const { return Suit(card / kCardsPerSuit); }
    constexpr Rank rank() const { return Rank(card % kCardsPerSuit); }

    constexpr uint64_t mask() const { return 1ull << card; }

    static constexpr auto cardFor(Suit s, Rank r) -> Card { return Card{Ord(Nib(s) * kCardsPerSuit + Nib(r))}; }

    friend bool operator==(Card a, Card b) { return a.card == b.card; }
    friend bool operator!=(Card a, Card b) { return a.card != b.card; }
    friend bool operator<(Card a, Card b) { return a.card < b.card; }

private:
    Ord card;
};

constexpr Card kNoCard{};

constexpr Suit suitOf(Card card) { return card.suit(); }
constexpr Rank rankOf(Card card) { return card.rank(); }
constexpr Card cardFor(Suit suit, Rank rank) { return Card::cardFor(suit, rank); }

constexpr Nib iSuitOf(Card card) { return Nib(suitOf(card)); }
constexpr Nib iRankOf(Card card) { return Nib(rankOf(card)); }

std::string nameOfSuit(Suit suit);
std::string nameOfCard(Card card);

using Trick = std::array<Card, kNumPlayers>;

} // namespace pho::cards
