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
    EXPECT_EQ(0u, cardFor(kClubs, kTwo));
    EXPECT_EQ(13u, cardFor(kDiamonds, kTwo));
    EXPECT_EQ(26u, cardFor(kSpades, kTwo));
    EXPECT_EQ(39u, cardFor(kHearts, kTwo));

    EXPECT_EQ(12u, cardFor(kClubs, kAce));
    EXPECT_EQ(25u, cardFor(kDiamonds, kAce));
    EXPECT_EQ(38u, cardFor(kSpades, kAce));
    EXPECT_EQ(51u, cardFor(kHearts, kAce));
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
        for (auto r = Nib(kTwo); r <= Nib(kAce); ++r)
        {
            bits |= cardFor(s, Rank(r)).mask();
        }
    }
    EXPECT_EQ(CardSet::kAllCards, bits);
}

TEST(Card, suitOf)
{
    EXPECT_EQ(kClubs, suitOf(0u));
    EXPECT_EQ(kDiamonds, suitOf(13u));
    EXPECT_EQ(kSpades, suitOf(26u));
    EXPECT_EQ(kHearts, suitOf(39u));

    EXPECT_EQ(kClubs, suitOf(12u));
    EXPECT_EQ(kDiamonds, suitOf(25u));
    EXPECT_EQ(kSpades, suitOf(38u));
    EXPECT_EQ(kHearts, suitOf(51u));
}

TEST(Card, rankOf)
{
    EXPECT_EQ(kTwo, rankOf(0u));
    EXPECT_EQ(kTwo, rankOf(13u));
    EXPECT_EQ(kTwo, rankOf(26u));
    EXPECT_EQ(kTwo, rankOf(39u));

    EXPECT_EQ(kAce, rankOf(12u));
    EXPECT_EQ(kAce, rankOf(25u));
    EXPECT_EQ(kAce, rankOf(38u));
    EXPECT_EQ(kAce, rankOf(51u));
}

TEST(Card, nameOfCard)
{
    EXPECT_EQ(std::string(" 2♣️"), nameOfCard(cardFor(kClubs, kTwo)));
    EXPECT_EQ(std::string(" 2♦️"), nameOfCard(cardFor(kDiamonds, kTwo)));
    EXPECT_EQ(std::string(" 2♠️"), nameOfCard(cardFor(kSpades, kTwo)));
    EXPECT_EQ(std::string(" 2♥️"), nameOfCard(cardFor(kHearts, kTwo)));

    EXPECT_EQ(std::string("10♣️"), nameOfCard(cardFor(kClubs, kTen)));
    EXPECT_EQ(std::string("10♦️"), nameOfCard(cardFor(kDiamonds, kTen)));
    EXPECT_EQ(std::string("10♠️"), nameOfCard(cardFor(kSpades, kTen)));
    EXPECT_EQ(std::string("10♥️"), nameOfCard(cardFor(kHearts, kTen)));

    EXPECT_EQ(std::string(" A♣️"), nameOfCard(cardFor(kClubs, kAce)));
    EXPECT_EQ(std::string(" A♦️"), nameOfCard(cardFor(kDiamonds, kAce)));
    EXPECT_EQ(std::string(" A♠️"), nameOfCard(cardFor(kSpades, kAce)));
    EXPECT_EQ(std::string(" A♥️"), nameOfCard(cardFor(kHearts, kAce)));
}

} // namespace
} // namespace pho::cards
