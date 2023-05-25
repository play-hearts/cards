#include "cards/CardSet.hpp"
#include "gtest/gtest.h"

#include <initializer_list>

namespace pho::cards::tests {

using CardsInitializerList = std::initializer_list<Card>;

static const auto kFiveCards = CardsInitializerList{0, 3, 6, 10, 51};

TEST(CardSet, default_ctor)
{
    CardSet hand;
    EXPECT_EQ(hand.size(), 0);
}

TEST(CardSet, make)
{
    auto hand = CardSet::make(kFiveCards);
    EXPECT_EQ(hand.size(), kFiveCards.size());
}

TEST(CardSet, fullDeck)
{
    auto deck = CardSet::fullDeck();
    EXPECT_EQ(52, deck.size());
    EXPECT_EQ(deck.front(), Card(0));
    EXPECT_EQ(deck.back(), Card(51));
}

TEST(CardSet, assignment)
{
    auto hand = CardSet::make(kFiveCards);

    CardSet other;
    other = hand;
    EXPECT_EQ(other.size(), kFiveCards.size());
    EXPECT_EQ(hand, other);
}

TEST(CardSet, operator_add_card)
{
    CardSet hand;
    EXPECT_EQ(0, hand.size());

    hand += Card{3};
    EXPECT_EQ(1, hand.size());
    EXPECT_EQ(Card{3}, hand.front());

    hand += Card{7};
    EXPECT_EQ(2, hand.size());
    EXPECT_EQ(Card{7}, hand.back());
}

TEST(CardSet, insert_ordered)
{
    CardSet hand;

    Card cards[] = {1, 7, 0};
    for (int i = 0; i < 3; i++)
        hand += cards[i];

    EXPECT_EQ(3, hand.size());
    for (int i = 0; i < 3; i++)
        EXPECT_TRUE(hand.hasCard(cards[i]));

    EXPECT_EQ(Card{0}, hand.front());
    EXPECT_EQ(Card{7}, hand.back());
}

TEST(CardSet, setUnion)
{
    auto hand1 = CardSet::make({0, 3, 6, 14, 21, 51});
    auto hand2 = CardSet::make({1, 2, 5, 15, 22, 50});

    hand1 += hand2;

    auto expected = CardSet::make({0, 1, 2, 3, 5, 6, 14, 15, 21, 22, 50, 51});
    EXPECT_EQ(expected, hand1);
}

TEST(CardSet, range_for)
{
    const auto cards = CardsInitializerList{1u, 3u, 6u, 14u, 21u, 51u};
    auto hand = CardSet::make(cards);

    auto it = cards.begin();
    for (Card c : hand)
    {
        EXPECT_EQ(c.ord(), (*it++).ord());
    }
    EXPECT_EQ(it, cards.end());
}

} // namespace pho::cards::tests
