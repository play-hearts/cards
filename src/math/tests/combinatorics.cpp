#include "math/combinatorics.hpp"
#include "math/Bits.hpp"
#include "gtest/gtest.h"
#include <set>

namespace pho::math::tests {

TEST(combinations128, small)
{
    EXPECT_EQ(1u, (unsigned)combinations128(0, 0));
    EXPECT_EQ(1u, (unsigned)combinations128(1, 1));
    EXPECT_EQ(3u, (unsigned)combinations128(3, 1));
    EXPECT_EQ(3u, (unsigned)combinations128(3, 2));
    EXPECT_EQ(4u, (unsigned)combinations128(4, 1));
    EXPECT_EQ(6u, (unsigned)combinations128(4, 2));
}

// As an independent confirmation, several of the values below are published here:
// http://www.rpbridge.net/7z68.htm

TEST(combinations128, waysFirstHand)
{
    uint128_t N = combinations128(52, 13);
    EXPECT_EQ(std::string("635013559600"), asDecimalString(N));
}

TEST(combinations128, waysSecondHand)
{
    uint128_t N = combinations128(39, 13);
    EXPECT_EQ(std::string("8122425444"), asDecimalString(N));
}

TEST(combinations128, waysThirdHand)
{
    uint128_t N = combinations128(26, 13);
    EXPECT_EQ(std::string("10400600"), asDecimalString(N));
}

TEST(combinations128, numTwoHands)
{
    uint128_t N = combinations128(52, 13) * combinations128(39, 13);
    EXPECT_EQ(std::string("5157850293780050462400"), asDecimalString(N));
}

TEST(combinations128, possibleDistinguishableDealsDec)
{
    uint128_t N = possibleDistinguishableDeals();
    EXPECT_EQ(std::string("53644737765488792839237440000"), asDecimalString(N));
}

TEST(combinations128, possibleDistinguishableDealsHex)
{
    uint128_t N = possibleDistinguishableDeals();
    EXPECT_EQ(std::string("ad55e315634dda658bf49200"), asHexString(N));
}

TEST(combinations128, possibleDistinguishableDealsHighBit)
{
    uint128_t N = possibleDistinguishableDeals();
    auto topBit = greatestSetBitIndex(N);
    EXPECT_EQ(topBit, 95);
}

TEST(asHexString, fill)
{
    uint128_t N = possibleDistinguishableDeals();
    EXPECT_EQ(std::string("ad55e315634dda658bf49200"), asHexString(N));
    EXPECT_EQ(std::string("ad55e315634dda658bf49200"), asHexString(N, 23));
    EXPECT_EQ(std::string("ad55e315634dda658bf49200"), asHexString(N, 24));
    EXPECT_EQ(std::string("0ad55e315634dda658bf49200"), asHexString(N, 25));
    EXPECT_EQ(std::string("00ad55e315634dda658bf49200"), asHexString(N, 26));
}

TEST(ChooseFromGenerator, nominal)
{
    ChooseFromGenerator<> gen;

    const auto kSize = gen.size();
    EXPECT_EQ(kSize, 286u);

    std::set<uint64_t> seen;
    for (unsigned i = 0; i < kSize; ++i)
    {
        auto mask = gen.next();

        // Sanity check
        EXPECT_LT(0u, mask);
        EXPECT_LT(mask, 1u << 13);

        // Tighter bounds
        EXPECT_LE(0b0111u, mask);
        EXPECT_LE(mask, 0b0111u << 10);

        EXPECT_EQ(seen.size(), i);
        EXPECT_EQ(seen.find(mask), seen.end());
        seen.insert(mask);
        EXPECT_EQ(seen.size(), i + 1);
    }

    auto last = gen.next();
    EXPECT_EQ(last, 0u);
}

} // namespace pho::math::tests
