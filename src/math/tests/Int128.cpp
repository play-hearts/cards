#include "math/Int128.hpp"
#include "gtest/gtest.h"
#include "math/random.hpp"
#include "prim/range.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif


namespace pho::math::tests {

constexpr auto kMaxUint126 = ~uint128_t(0) >> 2;

TEST(Int128, round_trip)
{
    for (auto i : prim::range(1000))
    {
        (void) i;
        auto x = RandomGenerator::Random128() & kMaxUint126;
        auto y = from_uint128(x);
        auto z = to_uint128(y);
        EXPECT_EQ(x, z);
    }
}

} // namespace pho::math::tests
