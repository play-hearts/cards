#include "gtest/gtest.h"

#include "cards/utils.hpp"
#include "gstate/GState.hpp"
#include "prim/range.hpp"
#include "math/random.hpp"

#include <fmt/ranges.h>

namespace pho::gstate {

using RandomGenerator = math::RandomGenerator;

auto passingSetup(GState& gameState)
{
    for (auto p : prim::range(kNumPlayers))
    {
        auto hand = gameState.playersHand(p);
        auto pass = chooseThreeAtRandom(hand);
        gameState.setPassFor(p, pass);
    }
}

TEST(GState, nominal)
{
    GState gameState{0, 0};

    gameState.startGame();

    while (!gameState.done())
    {
        auto legal = gameState.legalPlays();
        auto play = legal.front();
        gameState.playCard(play);
    }

    EXPECT_TRUE(gameState.done());
    auto outcome = gameState.getStandardOutcome();
    EXPECT_TRUE(outcome.mShooter == 0);
}

TEST(GState, random_deals_no_passing)
{
    for (auto i : prim::range(100))
    {
        (void)i;
        GState gameState{Deal::randomDealIndex(), 0};

        gameState.startGame();

        while (!gameState.done())
        {
            auto legal = gameState.legalPlays();
            auto play = legal.front();
            gameState.playCard(play);
        }

        EXPECT_TRUE(gameState.done());
        auto outcome = gameState.getStandardOutcome();
        auto shooter = outcome.shooter();
        if (outcome.shotTheMoon())
        {
            EXPECT_EQ(outcome.normalizedScore(shooter), -1.0);
        }
        else
        {
            EXPECT_EQ(shooter, 4u);
        }
    }
}

TEST(GState, random_deals_with_passing)
{
    for (uint8_t offset : prim::range(1u, 4u))
    {
        for (auto i : prim::range(20))
        {
            (void)i;
            GState gameState{Deal::randomDealIndex(), offset};

            passingSetup(gameState);
            gameState.startGame();

            while (!gameState.done())
            {
                auto legal = gameState.legalPlays();
                auto play = legal.front();
                gameState.playCard(play);
            }
        }
    }
}

TEST(GState, random_deals_with_passing_as_probs)
{
    for (uint8_t offset : prim::range(1u, 4u))
    {
        for (auto i : prim::range(20))
        {
            (void)i;
            GState gameState{Deal::randomDealIndex(), offset};

            passingSetup(gameState);
            gameState.startGame();

            while (!gameState.done())
            {
                auto legal = gameState.legalPlays();
                auto play = legal.front();

                auto probs = gameState.asProbabilities();
                EXPECT_LE(probs.at(0).at(0), 1.0);

                gameState.playCard(play);
            }
        }
    }
}

TEST(GState, fromConstrainedHandsPasses)
{
    auto carl = RandomGenerator::Range64(4);
    auto index = Deal::randomDealIndex();
    auto hands = Deal{index}.hands();
    auto carlsHand = hands.at(carl);
    auto passes = FourHands{};
    for (auto i : prim::range(4))
    {
        passes.at(i) = chooseThreeAtRandom(hands.at(i));
        EXPECT_EQ(passes.at(i).size(), 3);
    }
    auto passOffset = 1 + RandomGenerator::Range64(3);
    auto gState = GState::fromConstrainedHandsPasses(carl, carlsHand, passes, passOffset);
    gState.startGame();
}

} // namespace pho::gstate
