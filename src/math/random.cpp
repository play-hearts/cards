#include "math/random.hpp"
#include "prim/dlog.hpp"
#include "prim/range.hpp"

#include <assert.h>
#include <random>

namespace pho::math {

namespace {
DLog dlog("math_rng_seed");

class SplitMix64
{
    // https://xoshiro.di.unimi.it/splitmix64.c
    // This is only used by seedFrom64BitValue()
public:
    SplitMix64() = delete;
    SplitMix64(uint64_t seed)
    : x{seed}
    { }

    uint64_t next()
    {
        uint64_t z = (x += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

private:
    uint64_t x;
};
} // namespace

void RandomGenerator::seedFromRandomDevice()
{
    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dist{};
    for (auto i : prim::range(kStateWords))
    {
        s[i] = dist(rd);
        dlog("seedFromRandomDevice({}): {:x}", i, s[i]);
    }
}

RandomGenerator::RandomGenerator() { seedFromRandomDevice(); }

RandomGenerator::RandomGenerator(uint64_t seed) { seedFrom64BitValue(seed); }

void RandomGenerator::seedFrom64BitValue(uint64_t seed) const
{
    SplitMix64 mix{seed};
    s[0] = seed;
    s[1] = mix.next();
    s[2] = mix.next();
    s[3] = mix.next();
}

namespace {
inline uint64_t rotl(const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
} // namespace

uint64_t RandomGenerator::random64() const
{
    // http://prng.di.unimi.it/xoshiro256starstar.c
    const uint64_t result = rotl(s[1] * 5, 7) * 9;

    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 45);

    return result;
}

uint128_t RandomGenerator::random128() const
{
    uint128_t result = random64();
    return (result << 64) + random64();
}

uint64_t RandomGenerator::range64(uint64_t range) const
{
    const uint64_t buckets = kMax64 / range;
    const uint64_t limit = buckets * range;
    uint64_t r = random64();
    while (r >= limit)
        r = random64();
    return r / buckets;
}

uint128_t RandomGenerator::range128(uint128_t range) const
{
    const uint128_t buckets = kMax128 / range;
    const uint128_t limit = buckets * range;
    uint128_t r = random128();
    while (r >= limit)
        r = random128();
    return r / buckets;
}

double RandomGenerator::randNorm() const
{
    constexpr uint64_t kMaxMantissa{0x0010'0000'0000'0000ull};
    constexpr uint64_t kExponent{0x3ff0'0000'0000'0000ull};
    auto n = range64(kMaxMantissa) | kExponent;
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    double d = *reinterpret_cast<double*>(&n);
    assert(d >= 1.0);
    assert(d < 2.0);
    return d - 1.0;
}

thread_local RandomGenerator RandomGenerator::gRandomGenerator;

} // namespace pho::math
