#include "math/elo.hpp"

#include <cmath>

namespace pho::math {

auto eloDelta(double p) -> double
{
    if (p < 0.01)
        return -800;
    else if (p > 0.99)
        return 800;
    return std::log10(p / (1 - p)) * 400;
}

auto inverseEloDelta(double d) -> double { return 1 / (1 + std::pow(10, -d / 400)); }

} // namespace pho::math
