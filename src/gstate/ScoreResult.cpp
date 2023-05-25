#include "gstate/ScoreResult.hpp"
#include "gstate/GState.hpp"
#include "math/elo.hpp"
#include "stats/RunningStats.hpp"
#include "prim/dlog.hpp"

#include <fmt/format.h>

namespace pho::gstate {

namespace {
DLog dlog("ScoreResult");
}

ScoreResult& ScoreResult::operator+=(const ScoreResult& other)
{
    if (N == 0)
    {
        *this = other;
    }
    else
    {
        assert(kScoreType == other.kScoreType);
        mWinPts += other.mWinPts;
        mZms += other.mZms;
        N += other.N;
    }
    return *this;
}

ScoreResult& ScoreResult::soloUpdate(const GState& game, unsigned p)
{
    static thread_local auto stats = stats::RunningStats{};

    if (N == 0) kScoreType = eSolo;
    assert(kScoreType == eSolo);
    assert(p < 4u);
    auto result = game.getPlayerOutcome(p);

    mZms += result.zms;
    mWinPts += result.winPts;

    stats.accumulate(result.zms);
    if ((stats.size() % 100) == 0)
    {
        dlog("{}\n", toString(stats));
    }
    ++N;
    return *this;
}

ScoreResult& ScoreResult::teamUpdate(const GState& game, unsigned p1, unsigned p2)
{
    if (N == 0)
        kScoreType = eTeam;
    assert(kScoreType == eTeam);
    assert(p1 != p2);
    assert(p1 < 4 && p2 < 4);

    auto result1 = game.getPlayerOutcome(p1);
    auto result2 = game.getPlayerOutcome(p2);
    auto teamZms = result1.zms + result2.zms;
    auto teamWin = result1.winPts + result2.winPts;
    dlog("teamZms: {}\n", teamZms);
    dlog("teamWin: {}\n", teamWin);
    assert(teamWin>=0.0 && teamWin<=1.0);

    mZms += teamZms;
    mWinPts += teamWin;
    ++N;

    return *this;
}

auto ScoreResult::winFraction() const -> double
{
    if (N > 0)
        return double(mWinPts)  / N;
    else if (kScoreType == eSolo)
        return 0.25;
    else
        return 0.5;
}

auto ScoreResult::eloDelta() const -> double
{
    auto win = winFraction();
    if (kScoreType == eSolo)
        win *= 2.0;
    else
        win *= 1.0;
    return math::eloDelta(win);
}

auto ScoreResult::opponentWinFraction() const -> double
{
    auto win = 1.0 - winFraction();
    if (kScoreType == eSolo)
        win /= 3.0;
    return win;
}

auto ScoreResult::opponentElo() const -> double
{
    return -eloDelta();
}

auto ScoreResult::opponentScoreDelta() const -> double
{
    return -scoreDelta();
}

auto ScoreResult::opponentStrength() const -> std::tuple<double, double>
{
    return std::make_pair(opponentWinFraction(), opponentScoreDelta());
}

auto ScoreResult::operator<(const ScoreResult& other) const -> bool
{
    assert(kScoreType == other.kScoreType);
    return winFraction() < other.winFraction();
}

} // namespace pho::gstate
