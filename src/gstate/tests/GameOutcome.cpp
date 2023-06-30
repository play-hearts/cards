#include "gtest/gtest.h"

#include "cards/utils.hpp"
#include "gstate/GState.hpp"
#include "gstate/GameBehavior.hpp"
#include "prim/range.hpp"
#include "stats/RunningStats.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <magic_enum.hpp>
#include <map>
#include <numeric>

namespace pho::gstate {

template <typename T> auto uniform_loss(const std::array<T, 4>& values) -> double
{
    auto mean = std::accumulate(values.begin(), values.end(), 0) / 4.0;
    auto loss = double{};
    for (auto v : values)
    {
        auto x = v / mean - 1.0;
        loss += x * x;
    }
    return loss;
}

const auto atRandom = [](CardSet legal) -> Card { return aCardAtRandom(legal); };
const auto atFront = [](CardSet legal) -> Card { return legal.front(); };
const auto atBack = [](CardSet legal) -> Card { return legal.back(); };

using Chooser = std::function<Card(CardSet)>;

auto choosers = std::map<std::string, Chooser>({{"atRandom", atRandom}, {"atFront", atFront}, {"atBack", atBack}});

using GamePredicate = std::function<bool(const GState&)>;
const auto anyGame = [](const GState&) -> bool { return true; };
const auto moonShot = [](const GState& gameState) -> bool {
    auto outcome = gameState.getVariantOutcomeRep();
    return std::visit([](auto&& arg) { return arg.shotTheMoon(); }, outcome);
};
const auto noMoon = [](const GState& gameState) -> bool { return !moonShot(gameState); };

auto predicates
    = std::map<std::string, GamePredicate>({{"anyGame", anyGame}, {"moonShot", moonShot}, {"noMoon", noMoon}});

auto runOneGame(Chooser chooser, GamePredicate predicate, GameBehavior::Variant variant) -> GState
{
    while (true)
    {
        GState gameState{GState::kNoPass, GameBehavior::make(variant)};

        gameState.startGame();

        while (!gameState.done())
        {
            auto legal = gameState.legalPlays();
            auto play = chooser(legal);
            gameState.playCard(play);
        }

        if (predicate(gameState))
            return gameState;
    }
}

TEST(unbiased_by_seat, trick_lead)
{
    // This is equivalent to testing that the 2Clubs is unbiased by seat
    auto trickLeadCount = std::array<unsigned, kNumPlayers>{};
    for (auto i : prim::range(1000))
    {
        (void) i;
        GState gameState{GState::kNoPass};
        gameState.startGame();
        trickLeadCount.at(gameState.trickLead()) += 1;
    }
    EXPECT_LT(uniform_loss(trickLeadCount), 0.40);
}

TEST(unbiased_by_seat, player_outcome)
{
    for (auto variant : magic_enum::enum_values<GameBehavior::Variant>())
    {
        for (const auto& predicate : predicates)
        {
            if (predicate.first == "moonShot" && variant == GameBehavior::Variant::spades)
            {
                fmt::print("Skipping moon shot game with spades because never happens\n");
                continue;
            }
            for (const auto& chooser : choosers)
            {
                auto playerOutcome = std::array<double, kNumPlayers>{};
                auto stats = stats::RunningStats{};
                for (auto i : prim::range(100))
                {
                    (void) i;
                    auto gameState = runOneGame(chooser.second, predicate.second, variant);
                    auto outcome = gameState.getVariantOutcomeRep();
                    auto rowSum = float{};
                    auto scores = std::visit([](auto&& arg) { return arg.normalizedScores(); }, outcome);
                    for (auto p : prim::range(kNumPlayers))
                    {
                        auto score = std::visit([p](auto&& arg) { return arg.normalizedScore(p); }, outcome);
                        EXPECT_EQ(score, scores.at(p));
                        stats.accumulate(score);
                        auto x = std::visit([p](auto&& arg) { return arg.playerOutcomeResult(p); }, outcome);
                        EXPECT_GE(x, 0.0);
                        EXPECT_LE(x, 1.0);
                        playerOutcome.at(p) += x;
                        rowSum += x;
                    }
                    EXPECT_FLOAT_EQ(rowSum, 1.0f);
                }
                fmt::print("{}:{}:{}  score:{}\n", magic_enum::enum_name(variant), predicate.first, chooser.first,
                    toString(stats));
                auto avg = std::abs(stats.mean());
                EXPECT_LT(avg, 1.0e-6);
            }
        }
    }
}

} // namespace pho::gstate
