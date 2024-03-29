#pragma once

#include "cards/constants.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <variant>

namespace pho::gstate {

enum GameVariant
{
    standard,
    jack,
    spades
};

using FloatScores = std::array<float, cards::kNumPlayers>;

struct VariantOutcome
{
    struct Base
    {
        FloatScores mScores;
        auto normalizedScores() const -> FloatScores { return mScores; }
        auto normalizedScore(unsigned p) const -> float { return mScores[p]; }
        auto playerOutcomeResult(unsigned p) const -> float;
    };

    using PlayerNumber = uint32_t;
    static constexpr PlayerNumber kNoOne = cards::kNumPlayers;

    struct Standard : public Base
    {
        PlayerNumber mTookQueen{kNoOne};
        PlayerNumber mShooter{kNoOne};
        std::array<uint8_t, 4> mHeartsTaken;

        auto shooter() const -> PlayerNumber { return mShooter; }
        auto shotTheMoon() const -> bool { return mShooter != kNoOne; }
        auto tookQueen() const -> PlayerNumber { return mTookQueen; }
        auto heartsTaken(PlayerNumber p) const -> unsigned { return mHeartsTaken[p]; }
    };

    struct Jack : public Standard
    {
        PlayerNumber mTookJack{};
        auto tookJack() const -> PlayerNumber { return mTookJack; }
    };

    struct Spades : public Base
    {
        auto shotTheMoon() const -> bool { return false; }
    };
};

using VariantOutcomeRep = std::variant<VariantOutcome::Standard, VariantOutcome::Jack, VariantOutcome::Spades>;

} // namespace pho::gstate
