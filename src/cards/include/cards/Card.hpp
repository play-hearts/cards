// cards/Card.h

#pragma once

#include "cards/constants.hpp"

#include <array>
#include <string>

namespace pho::cards {

using Nib = std::int8_t; // We only need 2 bits, but we use 8. For representing Rank and Suit.
using Ord = std::uint8_t; // We only need 6 bits, but we use 8. For values 0..51.

#ifdef __EMSCRIPTEN__
#define USE_INT_SUIT_RANK 1
#else
#define USE_INT_SUIT_RANK 0
#endif

#if USE_INT_SUIT_RANK
using Suit = uint8_t;
constexpr Suit kClubs = 0;
constexpr Suit kDiamonds = 1;
constexpr Suit kSpades = 2;
constexpr Suit kHearts = 3;
constexpr Suit kUnknown = 4;
#else
enum Suit : Nib
{
    kClubs = 0,
    kDiamonds,
    kSpades,
    kHearts,

    kUnknown
};
#endif

constexpr auto allSuits = std::array<Suit, 4>{kClubs, kDiamonds, kSpades, kHearts};

#if USE_INT_SUIT_RANK
using Rank = uint8_t;
constexpr Rank kTwo = 0;
constexpr Rank kThree = 1;
constexpr Rank kFour = 2;
constexpr Rank kFive = 3;
constexpr Rank kSix = 4;
constexpr Rank kSeven = 5;
constexpr Rank kEight = 6;
constexpr Rank kNine = 7;
constexpr Rank kTen = 8;
constexpr Rank kJack = 9;
constexpr Rank kQueen = 10;
constexpr Rank kKing = 11;
constexpr Rank kAce = 12;
#else
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
#endif
class Card
{
public:
    static constexpr auto kNone = Ord{63};

    constexpr Card()
    : card{kNone}
    { }
    constexpr Card(Ord c)
    : card{c}
    { }

    Card(const Card&) = default;
    Card(Card&&) = default;
    Card& operator=(const Card&) = default;
    Card& operator=(Card&&) = default;

    constexpr Ord ord() const { return card; }
    constexpr Suit suit() const { return Suit(card / kCardsPerSuit); }
    constexpr Rank rank() const { return Rank(card % kCardsPerSuit); }

    constexpr uint64_t mask() const { return 1ull << card; }

    static constexpr auto cardFor(Suit s, Rank r) -> Card { return Card{Ord(s * kCardsPerSuit + r)}; }

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
constexpr Card cardFrom(Ord suit, Ord rank) { return Card::cardFor(Suit(suit), Rank(rank)); }

constexpr Nib iSuitOf(Card card) { return card.suit(); }
constexpr Nib iRankOf(Card card) { return card.rank(); }

std::string nameOfSuit(Suit suit);
std::string nameOfCard(Card card);

using Trick = std::array<Card, kNumPlayers>;

#if USE_INT_SUIT_RANK
static_assert(std::is_same_v<Suit, uint8_t>);
static_assert(std::is_same_v<Rank, uint8_t>);
#endif

} // namespace pho::cards
