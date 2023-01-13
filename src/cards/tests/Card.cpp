#include "gtest/gtest.h"

#include "math/Bits.hpp"
#include "cards/Card.hpp"
#include "cards/CardSet.hpp"

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
    EXPECT_EQ(0u, cardFor(kTwo, kClubs));
    EXPECT_EQ(13u, cardFor(kTwo, kDiamonds));
    EXPECT_EQ(26u, cardFor(kTwo, kSpades));
    EXPECT_EQ(39u, cardFor(kTwo, kHearts));

    EXPECT_EQ(12u, cardFor(kAce, kClubs));
    EXPECT_EQ(25u, cardFor(kAce, kDiamonds));
    EXPECT_EQ(38u, cardFor(kAce, kSpades));
    EXPECT_EQ(51u, cardFor(kAce, kHearts));
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
    EXPECT_EQ(std::string(" 2♣️"), nameOfCard(cardFor(kTwo, kClubs)));
    EXPECT_EQ(std::string(" 2♦️"), nameOfCard(cardFor(kTwo, kDiamonds)));
    EXPECT_EQ(std::string(" 2♠️"), nameOfCard(cardFor(kTwo, kSpades)));
    EXPECT_EQ(std::string(" 2♥️"), nameOfCard(cardFor(kTwo, kHearts)));

    EXPECT_EQ(std::string("10♣️"), nameOfCard(cardFor(kTen, kClubs)));
    EXPECT_EQ(std::string("10♦️"), nameOfCard(cardFor(kTen, kDiamonds)));
    EXPECT_EQ(std::string("10♠️"), nameOfCard(cardFor(kTen, kSpades)));
    EXPECT_EQ(std::string("10♥️"), nameOfCard(cardFor(kTen, kHearts)));

    EXPECT_EQ(std::string(" A♣️"), nameOfCard(cardFor(kAce, kClubs)));
    EXPECT_EQ(std::string(" A♦️"), nameOfCard(cardFor(kAce, kDiamonds)));
    EXPECT_EQ(std::string(" A♠️"), nameOfCard(cardFor(kAce, kSpades)));
    EXPECT_EQ(std::string(" A♥️"), nameOfCard(cardFor(kAce, kHearts)));
}

} // namespace anon
} // namespace pho::cards
