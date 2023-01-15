#pragma once

#include "math/Bits.hpp"
#include "math/math.hpp"

#include <assert.h>
#include <string>

namespace pho::math {

uint128_t combinations128(unsigned n, unsigned k);
// Returns n things taken k at a time.
// No attempt is made to detect overflow, but 128 bits is big enough for Hearts.

uint128_t possibleDistinguishableDeals();
// Returns 52! / 13!^4

template <unsigned N>
constexpr uint64_t constFactorial()
{
    static_assert(N < 15u);
    if constexpr(N == 0)
        return uint64_t{1};
    else
        return N * constFactorial<N-1>();
}

template <unsigned _K=3, unsigned _N=13>
class ChooseFromGenerator
{
public:

    static constexpr auto K = _K;
    static constexpr auto N = _N;

    // 0 < K <= N <= 13
    // Example: K=3, N=13 would generate all 286 ways to pick 3 from 13, i.e. possible ways to pass 3
    // Since this is intended to enumerate all possibilties, it doesn't make sense to use it large N.
    ChooseFromGenerator();

    // If return is 0, then generation is done.
    // Otherwise, the returned value will have K of the first N bits set,
    // i.e. valid results will be >= 2^K-1, and less  than 2^N-1, and CountBits(result) will K.
    uint64_t next();

    // The number of elements that should be generated
    static constexpr uint64_t size() { return constFactorial<N>() / (constFactorial<K>()*constFactorial<N-K>()); }

    static constexpr uint64_t first() { return ((1u<<K) - 1u); }
    static constexpr uint64_t last() { return first() << (N-K); }
    static constexpr uint64_t limit() { return last() + 1u; }

private:
    uint64_t mNext;
};

template <unsigned K, unsigned N>
ChooseFromGenerator<K, N>::ChooseFromGenerator()
: mNext(first())
{
    static_assert(0 < K);
    static_assert(K <= N);
    static_assert(N <= 13);

    assert(CountBits(first()) == K);
    assert(CountBits(last()) == K);
}

template <unsigned K, unsigned N>
uint64_t ChooseFromGenerator<K, N>::next()
{
    // Gosper's hack: https://en.wikipedia.org/wiki/Combinatorial_number_system#Applications
    if (mNext == 0)
        return mNext;

    assert(CountBits(mNext) == K);

    // We always have the next result queued up, so we will return the current value of mNext;
    auto result = mNext;

    // Now we use Gosper's hack to precompute the value that will be returned the next time.
    uint64_t& x{mNext}; // we alias mNext as `x` so the code can be (nearly) identical to the wiki code

    uint64_t u = x & -x; // extract rightmost bit 1; u =  0'00^a10^b
    uint64_t v = u + x; // set last non-trailing bit 0, and clear to the right; v=x'10^a00^b
    if (v==0) // then overflow in v, or x==0
        x = 0; // signal that next k-combination cannot be represented
    x = v +(((v^x)/u)>>2); // v^x = 0'11^a10^b, (v^x)/u = 0'0^b1^{a+2}, and x ← x'100^b1^a

    if (x > last())
        x = 0;

    assert(mNext != result);    // Just to prove our alias really did update mNext
    return result;
}

} // namespace pho::math
