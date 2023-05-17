#include "gtest/gtest.h"

#include "cards/Deal.hpp"
#include "cards/FourHands.hpp"
#include "prim/range.hpp"

namespace pho::cards::tests {

TEST(FourHands, ctor)
{
    auto hands = FourHands{};
    for (auto i : prim::range(kNumPlayers))
        EXPECT_TRUE(hands.at(i).empty());
}

TEST(FourHands, ctorWithRep)
{
    Deal deal;
    auto rep = FourHands::Rep{};
    for (auto i : prim::range(kNumPlayers))
        rep.at(i) = deal.dealFor(i);
    auto hands = FourHands{rep};
    for (auto i : prim::range(kNumPlayers))
        EXPECT_EQ(hands.at(i), rep.at(i));
}




} // namespace pho::cards::tests
