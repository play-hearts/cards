#include "gtest/gtest.h"

#include "cards/Deal.hpp"
#include "math/combinatorics.hpp"

namespace pho::cards::tests {

void dealIsValid(Deal deal)
{
    // Verify the four hands have no cards in common and combined include every card in the deck
    auto combined = CardSet{};
    EXPECT_TRUE(combined.empty());

    for (auto i : prim::range(kNumPlayers))
    {
        EXPECT_EQ(combined.size(), i*13);
        auto hand = deal.dealFor(i);
        EXPECT_TRUE(combined.setIntersection(hand).empty());
        combined += hand;
        EXPECT_EQ(combined.size(), (i+1)*13);
    }

    EXPECT_EQ(combined.size(), kCardsPerDeck);
}

TEST(Deal, defaultConstructor)
{
    Deal deal;
    dealIsValid(deal);
}

TEST(Deal, constructWithRandomIndex)
{
    auto index = Deal::randomDealIndex();
    Deal deal(index);
    dealIsValid(deal);
}

TEST(Deal, deckForIndex0)
{
    Deal deal(0);
    dealIsValid(deal);
    for (int i = 0; i < 52; i++)
        EXPECT_EQ(i, deal.PeekAt(i));
}

TEST(Deal, deckForIndex1)
{
    Deal deal(1);
    dealIsValid(deal);
    for (int i = 0; i < 38; i++)
        EXPECT_EQ(i, deal.PeekAt(i));
    for (int i = 40; i < 52; i++)
        EXPECT_EQ(i, deal.PeekAt(i));
    EXPECT_EQ(39, deal.PeekAt(38));
    EXPECT_EQ(38, deal.PeekAt(39));
}

TEST(Deal, deckForIndex2)
{
    Deal deal(2);
    dealIsValid(deal);
    for (int i = 0; i < 38; i++)
        EXPECT_EQ(i, deal.PeekAt(i));
    for (int i = 41; i < 52; i++)
        EXPECT_EQ(i, deal.PeekAt(i));
    EXPECT_EQ(40, deal.PeekAt(38));
    EXPECT_EQ(38, deal.PeekAt(39));
    EXPECT_EQ(39, deal.PeekAt(40));
}

TEST(Deal, dealFromHexString)
{
    EXPECT_EQ(Deal("0").dealIndex(), 0);
    EXPECT_EQ(Deal("1").dealIndex(), 1);
    EXPECT_EQ(Deal("0fff").dealIndex(), 0xfff);

    Deal deal("");
    dealIsValid(deal);
    const auto index = math::asHexString(deal.dealIndex());
    EXPECT_GT(index.size(), 10);    // just a very cheap & dirty test for randomness

    for (int i=0; i<10; ++i)
    {
        Deal d("");
        EXPECT_NE(d.dealIndex(), deal.dealIndex());
        dealIsValid(d);
    }
}

TEST(Deal, deckWithLastIndex)
{
    const auto index = math::possibleDistinguishableDeals() - 1;

    Deal deal(index);
    dealIsValid(deal);
    for (int suit = 0; suit < 4; suit++)
    {
        for (int rank = 0; rank < 13; rank++)
        {
            int i = suit * 13 + rank;
            int expectedSuit = 3 - suit;
            int expectedCard = expectedSuit * 13 + rank;
            EXPECT_EQ(expectedCard, deal.PeekAt(i));
        }
    }
}

} // namespace pho::cards::tests
