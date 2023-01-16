#pragma once

#include "math/math.hpp"
#include <stdint.h>

namespace pho::math {

inline int leastSetBitIndex(uint64_t x) { return x == 0 ? 64 : __builtin_ctzll(x); }

inline int greatestSetBitIndex(uint64_t x) { return x == 0 ? -1 : 63 - __builtin_clzll(x); }

inline int greatestSetBitIndex(uint128_t x)
{
    auto hiWordBit = greatestSetBitIndex(uint64_t(x >> 64));
    return hiWordBit != 64 ? 64u + hiWordBit : greatestSetBitIndex(uint64_t(x));
}

inline unsigned countBits(uint64_t x) { return __builtin_popcountll(x); }

inline uint64_t isolateLeastBit(uint64_t x) { return uint64_t{1} << leastSetBitIndex(x); }

inline uint64_t isolateGreatestBit(uint64_t x) { return uint64_t{1} << greatestSetBitIndex(x); }

inline uint64_t roundUpToPowerOfTwo(uint64_t n)
{
    uint64_t b = uint64_t{1} << greatestSetBitIndex(n);
    if (b < n)
        b *= 2;
    return b;
}

} // namespace pho::math
