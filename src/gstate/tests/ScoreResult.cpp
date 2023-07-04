#include "gtest/gtest.h"

#include "cards/utils.hpp"
#include "gstate/GState.hpp"
#include "gstate/ScoreResult.hpp"
#include "prim/range.hpp"
#include "stats/RunningStats.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace pho::gstate {
using namespace pho::cards;

auto runOneGame() -> GState
{
    while (true)
    {
        GState gameState{GState::kNoPass};

        gameState.startGame();

        while (!gameState.done())
        {
            auto legal = gameState.legalPlays();
            auto play = legal.front();
            gameState.playCard(play);
        }

        EXPECT_TRUE(gameState.done());

        if (!gameState.getStandardOutcome().shotTheMoon())
            continue;
        return gameState;
    }
}

TEST(ScoreResult, solo_zero_sum_win_fraction)
{
    // When we sum all four players results for one game (as if we played four games)
    // then  the win fraction should be exactly 25%.
    // We also should see the opponent at 25%, since the opponent win rate is the average opponent win rate.
    for (auto i : prim::range(100))
    {
        (void)i;
        auto gameState = runOneGame();

        auto scoreResult = ScoreResult{};
        EXPECT_EQ(scoreResult.count(), 0u);
        EXPECT_EQ(scoreResult.scoreType(), ScoreResult::eUnknown);
        for (auto p : prim::range(kNumPlayers))
        {
            scoreResult.soloUpdate(gameState, p);
            EXPECT_EQ(scoreResult.scoreType(), ScoreResult::eSolo);
        }

        EXPECT_EQ(scoreResult.count(), 4);
        EXPECT_EQ(scoreResult.winFraction(), 0.25);
        EXPECT_EQ(scoreResult.opponentWinFraction(), 0.25);
    }
}

TEST(ScoreResult, team_zero_sum_win_fraction)
{
    // When we sum both team results for one game (as if we played two games)
    // then  the win fraction should be exactly 50%.
    for (auto i : prim::range(100))
    {
        (void)i;
        auto gameState = runOneGame();

        ScoreResult scoreResult{};
        scoreResult.teamUpdate(gameState, 0, 2);
        scoreResult.teamUpdate(gameState, 1, 3);

        EXPECT_EQ(scoreResult.winFraction(), 0.5);
        EXPECT_EQ(scoreResult.opponentWinFraction(), 0.5);
    }
}

TEST(ScoreResult, solo_range)
{
    std::array<ScoreResult, kNumPlayers> scoreResults;
    for (auto i : prim::range(100))
    {
        (void)i;
        auto gameState = runOneGame();

        for (auto p : prim::range(kNumPlayers))
            scoreResults.at(p).soloUpdate(gameState, p);
    }

    auto sum = double{0.0};
    auto stats = stats::RunningStats{};
    for (auto p : prim::range(kNumPlayers))
    {
        auto win = scoreResults.at(p).winFraction();
        sum += win;
        stats.accumulate(win);
    }
    EXPECT_DOUBLE_EQ(sum, 1.0);

    // The win stats mean seems to always be around 0.25 +/- 0.02.
    fmt::print("***solo win stats: {}\n", toString(stats));
}

TEST(ScoreResult, solo_expected_win_fraction)
{
    auto stats = std::array<stats::RunningStats, kNumPlayers>{};
    for (auto i : prim::range(1000))
    {
        (void)i;
        auto gameState = runOneGame();

        auto zeroSumResult = ScoreResult{};
        for (auto p : prim::range(kNumPlayers))
        {
            auto scoreResult = ScoreResult{};
            scoreResult.soloUpdate(gameState, p);
            zeroSumResult.soloUpdate(gameState, p);
            stats.at(p).accumulate(scoreResult.winFraction());
        }

        EXPECT_EQ(zeroSumResult.winFraction(), 0.25);
    }
    for (auto p : prim::range(kNumPlayers))
        fmt::print("{}\n", toString(stats.at(p)));
}

TEST(ScoreResult, team_range)
{
    stats::RunningStats stats;
    for (auto i : prim::range(100))
    {
        (void)i;
        auto gameState = runOneGame();

        ScoreResult scoreResultA{};
        ScoreResult scoreResultB{};
        scoreResultA.teamUpdate(gameState, 0, 2);
        scoreResultB.teamUpdate(gameState, 1, 3);
        stats.accumulate(scoreResultA.winFraction());
        stats.accumulate(scoreResultB.winFraction());
    }

    EXPECT_EQ(stats.minimum(), 0.0);
    EXPECT_EQ(stats.maximum(), 1.0);

    fmt::print("team mean: {}\n", stats.mean());
}

TEST(ScoreResult, solo_elo_average)
{
    auto ok = false;
    ScoreResult allRuns{};
    for (auto run : prim::range(10))
    {
        (void)run;
        ScoreResult scoreResult{};
        for (auto i : prim::range(100))
        {
            (void)i;
            auto gameState = runOneGame();
            scoreResult.soloUpdate(gameState, 0);
        }
        allRuns += scoreResult;

        // This test is expected to occasionally fail as the actual distribution of scores
        // has tails that go beyond this arbitrary range.
        // We could reduce the failure rate by running a larger number of games, but then
        // the test will be slow to execute. Instead, we just retry until it passes.
        auto elo = scoreResult.eloDelta();
        ok = elo > -100 && elo < 100;
        if (ok)
            break;
    }
    EXPECT_TRUE(ok);

    auto elo = allRuns.eloDelta();
    fmt::print("solo elo: {:.1f}\n", elo);
}

TEST(ScoreResult, team_elo_average)
{
    ScoreResult scoreResult{};
    for (auto i : prim::range(1000))
    {
        (void)i;
        auto gameState = runOneGame();
        scoreResult.teamUpdate(gameState, 0, 2);
    }

    auto elo = scoreResult.eloDelta();
    fmt::print("team elo: {:.1f}\n", elo);

    EXPECT_GT(elo, -50);
    EXPECT_LT(elo, 50);
}

} // namespace pho::gstate
