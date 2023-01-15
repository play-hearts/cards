#pragma once

#include "math/math.hpp"

namespace pho::math {

class RandomGenerator
{
public:
    /// @brief Create a PRNG, seeded from std::random_device.
    RandomGenerator();

    /// @brief  Create a PRNG, seeded from the given 64-bit seed.
    /// @param seed
    RandomGenerator(uint64_t seed);

    RandomGenerator(const RandomGenerator&) = default;

    bool operator==(const RandomGenerator& o) const
    {
        return s[0] == o.s[0]
            && s[1] == o.s[1]
            && s[2] == o.s[2]
            && s[3] == o.s[3];
    }

    bool operator!=(const RandomGenerator& o) const { return !this->operator==(o); }

    uint64_t random64() const;

    uint64_t range64(uint64_t range) const;

    uint128_t random128() const;

    uint128_t range128(uint128_t range) const;

    double randNorm() const;

public:
    static const RandomGenerator& ThreadSpecific() { return gRandomGenerator; }

    static uint64_t Random64() { return gRandomGenerator.random64(); }

    static uint64_t Range64(uint64_t range) { return gRandomGenerator.range64(range); }

    static uint128_t Random128() { return gRandomGenerator.random128(); }

    static uint128_t Range128(uint128_t range) { return gRandomGenerator.range128(range); }

    static double RandNorm() { return gRandomGenerator.randNorm(); }

public:
    static constexpr uint64_t kZero64 = 0;
    static constexpr uint64_t kMax64 = ~kZero64;

    static constexpr uint128_t kZero128 = 0;
    static constexpr uint128_t kMax128 = ~kZero128;

private:
    // The state must be seeded so that it is not everywhere zero.
    static constexpr int kStateWords{4};
    mutable uint64_t s[kStateWords];

    void seedFromRandomDevice();

    void seedFrom64BitValue(uint64_t seed) const;

private:
    static thread_local RandomGenerator gRandomGenerator;
};

} // namespace pho::math
