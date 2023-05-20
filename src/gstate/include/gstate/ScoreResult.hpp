#pragma once

#include <cstdint>
#include <cassert>
#include <tuple>

namespace pho::gstate {

class GState;

/// @brief ScoreResult is an accumulator of game outcome scores for two metrics:
/// 1. Win rate
/// 2. Zero Mean Score.
/// We support both :"solo" and "team" scoring.

class ScoreResult
{
public:
    enum ScoreType { eUnknown, eSolo, eTeam };

    ~ScoreResult() = default;

    ScoreResult()
    : mWinPts{}
    , mZms{}
    , N{}
    , kScoreType{eUnknown}
    {}

    ScoreResult(const ScoreResult&) = default;
    ScoreResult(ScoreResult&&) = default;

    ScoreResult& operator=(const ScoreResult&) = default;
    ScoreResult& operator=(ScoreResult&&) = default;

    ScoreResult& operator+=(const ScoreResult& other);

    ScoreResult& soloUpdate(const GState& game, unsigned p);

    ScoreResult& teamUpdate(const GState& game, unsigned p1, unsigned p2);

    auto count() const -> uint64_t { return N; }

    auto winFraction() const -> double;

    auto scoreDelta() const -> double { return N==0 ? 0.0 : mZms/N; }

    auto eloDelta() const -> double;

    auto opponentWinFraction() const -> double;

    auto opponentElo() const -> double;

    auto opponentScoreDelta() const -> double;

    auto opponentStrength() const -> std::tuple<double, double>;

    auto operator<(const ScoreResult& other) const -> bool;

    auto scoreType() const -> ScoreType { return kScoreType; }

private:

    double mWinPts;
    double mZms;
    uint64_t N;
    ScoreType kScoreType;
};

} // namespace pho::gstate
