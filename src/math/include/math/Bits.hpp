#pragma once

#include <stdint.h>
#include "math/math.hpp"
#include "math/Bits.hpp"

namespace pho::math {

inline int LeastSetBitIndex(uint64_t x) {
    return x==0 ? 64 : __builtin_ctzll(x);
}

inline int GreatestSetBitIndex(uint64_t x)
{
    return x==0 ? -1 : 63 - __builtin_clzll(x);
}

inline int GreatestSetBitIndex(uint128_t x)
{
    auto hiWordBit = GreatestSetBitIndex(uint64_t(x >> 64));
    return hiWordBit != 64 ? 64u+hiWordBit : GreatestSetBitIndex(uint64_t(x));
}

inline unsigned CountBits(uint64_t x)
{
    return __builtin_popcountll(x);
}

inline uint64_t IsolateLeastBit(uint64_t x) { return uint64_t{1} << LeastSetBitIndex(x); }

inline uint64_t IsolateGreatestBit(uint64_t x) { return uint64_t{1} << GreatestSetBitIndex(x); }

inline uint64_t RoundUpToPowerOfTwo(uint64_t n)
{
    uint64_t b = uint64_t{1} << GreatestSetBitIndex(n);
    if (b < n)
        b *= 2;
    return b;
}

} // namespace pho::math
