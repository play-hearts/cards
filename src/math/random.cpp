#include "math/random.hpp"

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace pho::math {

RandomGenerator::RandomGenerator()
{
    const int kNumBytes = sizeof(s);
    int fd = open("/dev/urandom", O_RDONLY);
    assert(fd != -1);
    int actual = read(fd, s, kNumBytes);
    close(fd);
    if (actual != kNumBytes)
    {
        fprintf(stderr, "Failed to read enough bytes to initialize RandomGenerator\n");
        exit(1);
    }
}

RandomGenerator::RandomGenerator(uint128_t seed)
{
    reseed(seed);
}

void RandomGenerator::reseed(uint128_t seed) const
{
    s[0] = seed;
    s[1] = seed >> 21;
    s[2] = seed >> 42;
    s[3] = seed >> 64;
}

namespace {
inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}
}

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
    double d = *reinterpret_cast<double *>(&n);
    assert(d >= 1.0);
    assert(d < 2.0);
    return d - 1.0;
}

thread_local RandomGenerator RandomGenerator::gRandomGenerator;

} // namespace pho::math
