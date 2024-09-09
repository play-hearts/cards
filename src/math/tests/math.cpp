#include "math/math.hpp"
#include "math/random.hpp"
#include "gtest/gtest.h"

namespace pho::math::tests {

TEST(asHexString_parseHexString, RoundTrip)
{
    for (int i = 0; i < 100; ++i)
    {
        auto x = RandomGenerator::Random128();
        auto s = asHexString(x);
        auto z = parseHex128(s);
        EXPECT_EQ(x, z);
    }
}

} // pho::math::tests
