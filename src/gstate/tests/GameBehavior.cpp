#include "gtest/gtest.h"

#include "gstate/GameBehavior.hpp"
#include "gstate/GState.hpp"
#include "gstate/Trick.hpp"

namespace pho::gstate {

TEST(GameBehavior, factory)
{
    auto standard = GameBehavior::make(GameVariant::standard);
    auto jack = GameBehavior::make(GameVariant::jack);
}

TEST(GameBehavior_Standard, legal)
{
    auto standard = GameBehavior::make(GameVariant::standard);

    auto gameState = GState{GState::kNoPass};
    gameState.startGameNoPass();
    auto legal = standard.legal(gameState);
    EXPECT_EQ(legal.size(), 1u);
    EXPECT_EQ(legal.front(), Card::cardFor(Suit::kClubs, Rank::kTwo));
}

constexpr auto k2C = Card::cardFor(Suit::kClubs, Rank::kTwo);
constexpr auto k3C = Card::cardFor(Suit::kClubs, Rank::kThree);
constexpr auto k4C = Card::cardFor(Suit::kClubs, Rank::kFour);

constexpr auto k2D = Card::cardFor(Suit::kDiamonds, Rank::kTwo);

constexpr auto k2S = Card::cardFor(Suit::kSpades, Rank::kTwo);
constexpr auto k3S = Card::cardFor(Suit::kSpades, Rank::kThree);
constexpr auto k4S = Card::cardFor(Suit::kSpades, Rank::kFour);

TEST(GameBehavior, trickWinner)
{
    auto standard = GameBehavior::make(GameVariant::standard);
    auto spades = GameBehavior::make(GameVariant::spades);

    EXPECT_EQ(standard.trickWinner(Trick{{k2C, k3C, k2S, k4C}}), 3);
    EXPECT_EQ(standard.trickWinner(Trick{{k2S, k3S, k4S, k4C}}), 2);
    EXPECT_EQ(spades.trickWinner(Trick{{k2C, k3C, k2S, k4C}}), 2);

    EXPECT_EQ(standard.trickWinner(Trick{{k2C, k3C, k2S, k2D}}), 1);
    EXPECT_EQ(spades.trickWinner(Trick{{k2C, k3C, k2S, k2D}}), 2);

}


} // pho::gstate
