#include "gtest/gtest.h"

#include "cards/Card.hpp"
#include "cards/CardSet.hpp"
#include "math/Bits.hpp"

namespace pho::cards {
namespace {

TEST(Card, constants)
{
    EXPECT_EQ(4u, kSuitsPerDeck);
    EXPECT_EQ(13u, kCardsPerSuit);
    EXPECT_EQ(52u, kCardsPerDeck);
}

TEST(Card, cardFor)
{
    EXPECT_EQ(0u, cardFor(Suit::kClubs, Rank::kTwo));
    EXPECT_EQ(13u, cardFor(Suit::kDiamonds, Rank::kTwo));
    EXPECT_EQ(26u, cardFor(Suit::kSpades, Rank::kTwo));
    EXPECT_EQ(39u, cardFor(Suit::kHearts, Rank::kTwo));

    EXPECT_EQ(12u, cardFor(Suit::kClubs, Rank::kAce));
    EXPECT_EQ(25u, cardFor(Suit::kDiamonds, Rank::kAce));
    EXPECT_EQ(38u, cardFor(Suit::kSpades, Rank::kAce));
    EXPECT_EQ(51u, cardFor(Suit::kHearts, Rank::kAce));
}

TEST(Card, construct_all_cards)
{
    for (auto i = 0u; i < kCardsPerDeck; ++i)
    {
        EXPECT_EQ(i, Card(i).ord());
    }
}

TEST(Card, construct_all_cards_by_suit_rank)
{
    uint64_t bits = 0;
    for (auto s : allSuits)
    {
        for (auto r = Nib(Rank::kTwo); r <= Nib(Rank::kAce); ++r)
        {
            bits |= cardFor(s, Rank(r)).mask();
        }
    }
    EXPECT_EQ(CardSet::kAllCards, bits);
}

TEST(Card, suitOf)
{
    EXPECT_EQ(Suit::kClubs, suitOf(0u));
    EXPECT_EQ(Suit::kDiamonds, suitOf(13u));
    EXPECT_EQ(Suit::kSpades, suitOf(26u));
    EXPECT_EQ(Suit::kHearts, suitOf(39u));

    EXPECT_EQ(Suit::kClubs, suitOf(12u));
    EXPECT_EQ(Suit::kDiamonds, suitOf(25u));
    EXPECT_EQ(Suit::kSpades, suitOf(38u));
    EXPECT_EQ(Suit::kHearts, suitOf(51u));
}

TEST(Card, rankOf)
{
    EXPECT_EQ(Rank::kTwo, rankOf(0u));
    EXPECT_EQ(Rank::kTwo, rankOf(13u));
    EXPECT_EQ(Rank::kTwo, rankOf(26u));
    EXPECT_EQ(Rank::kTwo, rankOf(39u));

    EXPECT_EQ(Rank::kAce, rankOf(12u));
    EXPECT_EQ(Rank::kAce, rankOf(25u));
    EXPECT_EQ(Rank::kAce, rankOf(38u));
    EXPECT_EQ(Rank::kAce, rankOf(51u));
}

TEST(Card, nameOfCard)
{
    EXPECT_EQ(std::string(" 2♣️"), nameOfCard(cardFor(Suit::kClubs, Rank::kTwo)));
    EXPECT_EQ(std::string(" 2♦️"), nameOfCard(cardFor(Suit::kDiamonds, Rank::kTwo)));
    EXPECT_EQ(std::string(" 2♠️"), nameOfCard(cardFor(Suit::kSpades, Rank::kTwo)));
    EXPECT_EQ(std::string(" 2♥️"), nameOfCard(cardFor(Suit::kHearts, Rank::kTwo)));

    EXPECT_EQ(std::string("10♣️"), nameOfCard(cardFor(Suit::kClubs, Rank::kTen)));
    EXPECT_EQ(std::string("10♦️"), nameOfCard(cardFor(Suit::kDiamonds, Rank::kTen)));
    EXPECT_EQ(std::string("10♠️"), nameOfCard(cardFor(Suit::kSpades, Rank::kTen)));
    EXPECT_EQ(std::string("10♥️"), nameOfCard(cardFor(Suit::kHearts, Rank::kTen)));

    EXPECT_EQ(std::string(" A♣️"), nameOfCard(cardFor(Suit::kClubs, Rank::kAce)));
    EXPECT_EQ(std::string(" A♦️"), nameOfCard(cardFor(Suit::kDiamonds, Rank::kAce)));
    EXPECT_EQ(std::string(" A♠️"), nameOfCard(cardFor(Suit::kSpades, Rank::kAce)));
    EXPECT_EQ(std::string(" A♥️"), nameOfCard(cardFor(Suit::kHearts, Rank::kAce)));
}

} // namespace
} // namespace pho::cards
