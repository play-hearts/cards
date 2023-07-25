#include "math/Bits.hpp"
#include "gtest/gtest.h"

namespace pho::math::tests {

const uint64_t kZero = 0;
const uint64_t kOne = 1;

TEST(leastSetBitIndex, OneBit)
{
    for (int i = 0; i < 64; ++i)
    {
        EXPECT_EQ(i, leastSetBitIndex(kOne << i));
    }
}

TEST(greatestSetBitIndex, OneBit)
{
    for (int i = 0; i < 64; ++i)
    {
        EXPECT_EQ(i, greatestSetBitIndex(kOne << i));
    }
}

TEST(isolateLeastBit, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        const uint64_t bits = kOne << i;
        EXPECT_EQ(bits, isolateLeastBit(bits));
    }
}

TEST(isolateGreatestBit, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        const uint64_t bits = kOne << i;
        EXPECT_EQ(bits, isolateGreatestBit(bits));
    }
}

TEST(leastSetBitIndex, TwoBits)
{
    const uint64_t bits = 3;
    for (int i = 0; i < 63; ++i)
    {
        EXPECT_EQ(i, leastSetBitIndex(bits << i));
    }
}

TEST(greatestSetBitIndex, TwoBits)
{
    const uint64_t bits = 3;
    for (int i = 0; i < 63; ++i)
    {
        EXPECT_EQ(i + 1, greatestSetBitIndex(bits << i));
    }
}

TEST(leastSetBitIndex, Zero) { EXPECT_EQ(64, leastSetBitIndex(kZero)); }

TEST(greatestSetBitIndex, Zero) { EXPECT_EQ(-1, greatestSetBitIndex(kZero)); }

TEST(greatestSetBitIndex, uint128)
{
    const auto kHuge = ~uint128_t{0};
    EXPECT_EQ(greatestSetBitIndex(kHuge), 127);
}

TEST(countBits, nominal)
{
    EXPECT_EQ(0u, countBits(kZero));
    EXPECT_EQ(1u, countBits(8));
    EXPECT_EQ(2u, countBits(12));
    EXPECT_EQ(3u, countBits(82));
    EXPECT_EQ(52u, countBits((kOne << 52) - 1));
    EXPECT_EQ(64u, countBits(~kZero));
}

TEST(roundUpToPowerOfTwo, Exact)
{
    for (uint64_t i = 0; i < 64; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u, roundUpToPowerOfTwo(u));
    }
}

TEST(roundUpToPowerOfTwo, RoundUp1)
{
    for (uint64_t i = 0; i < 63; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u * 2, roundUpToPowerOfTwo(u + 1));
    }
}

TEST(roundUpToPowerOfTwo, RoundUp2)
{
    for (uint64_t i = 2; i < 64; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u, roundUpToPowerOfTwo(u - 1));
    }
}

} // namespace pho::math::tests
