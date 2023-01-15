#include "gtest/gtest.h"
#include "math/Bits.hpp"

namespace pho::math::tests {

const uint64_t kZero = 0;
const uint64_t kOne = 1;

TEST(LeastSetBitIndex, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        EXPECT_EQ(i, LeastSetBitIndex(kOne << i));
    }
}

TEST(GreatestSetBitIndex, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        EXPECT_EQ(i, GreatestSetBitIndex(kOne << i));
    }
}

TEST(IsolateLeastBit, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        const uint64_t bits = kOne << i;
        EXPECT_EQ(bits, IsolateLeastBit(bits));
    }
}

TEST(IsolateGreatestBit, OneBit)
{
    for (unsigned i = 0; i < 64; ++i)
    {
        const uint64_t bits = kOne << i;
        EXPECT_EQ(bits, IsolateGreatestBit(bits));
    }
}

TEST(LeastSetBitIndex, TwoBits)
{
    const uint64_t bits = 3;
    for (unsigned i = 0; i < 63; ++i)
    {
        EXPECT_EQ(i, LeastSetBitIndex(bits << i));
    }
}

TEST(GreatestSetBitIndex, TwoBits)
{
    const uint64_t bits = 3;
    for (unsigned i = 0; i < 63; ++i)
    {
        EXPECT_EQ(i + 1, GreatestSetBitIndex(bits << i));
    }
}

TEST(LeastSetBitIndex, Zero) { EXPECT_EQ(64, LeastSetBitIndex(kZero)); }

TEST(GreatestSetBitIndex, Zero) { EXPECT_EQ(-1, GreatestSetBitIndex(kZero)); }

TEST(GreatestSetBitIndex, uint128)
{
    const auto kHuge = ~uint128_t{0};
    EXPECT_EQ(GreatestSetBitIndex(kHuge), 127);
}

TEST(CountBits, nominal)
{
    EXPECT_EQ(0, CountBits(kZero));
    EXPECT_EQ(1, CountBits(8));
    EXPECT_EQ(2, CountBits(12));
    EXPECT_EQ(3, CountBits(82));
    EXPECT_EQ(52, CountBits((kOne << 52) - 1));
    EXPECT_EQ(64, CountBits(~kZero));
}

TEST(RoundUpToPowerOfTwo, Exact) {
    for (uint64_t i=0; i<64; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u, RoundUpToPowerOfTwo(u));
    }
}

TEST(RoundUpToPowerOfTwo, RoundUp1)
{
    for (uint64_t i = 0; i < 63; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u * 2, RoundUpToPowerOfTwo(u + 1));
    }
}

TEST(RoundUpToPowerOfTwo, RoundUp2)
{
    for (uint64_t i = 2; i < 64; ++i)
    {
        uint64_t u = kOne << i;
        EXPECT_EQ(u, RoundUpToPowerOfTwo(u - 1));
    }
}

} // namespace pho::math::tests
