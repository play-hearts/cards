#include "math/elo.hpp"
#include "gtest/gtest.h"

namespace pho::math::tests {

TEST(elo, nominal)
{
    EXPECT_EQ(0.0, eloDelta(0.5));
    EXPECT_EQ(800.0, eloDelta(1));
    EXPECT_EQ(-800.0, eloDelta(0));

    EXPECT_DOUBLE_EQ(24.999999999999979, eloDelta(0.53591592694510226));
    EXPECT_DOUBLE_EQ(-25.000000000000043, eloDelta(0.46408407305489768));

    EXPECT_GT(-400.0, eloDelta(0.05));
    EXPECT_LT(-800.0, eloDelta(0.05));

    EXPECT_LT(400.0, eloDelta(0.95));
    EXPECT_GT(800.0, eloDelta(0.95));
}

TEST(inverseEloDelta, nominal)
{
    EXPECT_EQ(0.5, inverseEloDelta(0));
    EXPECT_FLOAT_EQ(0.990099, inverseEloDelta(800));
    EXPECT_FLOAT_EQ(0.00990099, inverseEloDelta(-800));

    EXPECT_DOUBLE_EQ(0.53591592694510226, inverseEloDelta(25));
    EXPECT_DOUBLE_EQ(0.46408407305489768, inverseEloDelta(-25));

    EXPECT_DOUBLE_EQ(0.90909090909090906, inverseEloDelta(400));
    EXPECT_DOUBLE_EQ(0.090909090909090912, inverseEloDelta(-400));

    EXPECT_LT(0.05, inverseEloDelta(-400));
    EXPECT_GT(0.05, inverseEloDelta(-800));

    EXPECT_GT(0.95, inverseEloDelta(400));
    EXPECT_LT(0.95, inverseEloDelta(800));}

} // namespace pho::math::tests
