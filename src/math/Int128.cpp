#include "math/Int128.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include <cassert>

namespace pho::math {

constexpr auto kMaxUint63 = ~uint64_t(0) >> 1;
constexpr auto kMaxUint126 = ~uint128_t(0) >> 2;

auto to_uint128(const Int128& t) -> uint128_t
{
    assert(t.hi <= kMaxUint63);
    assert(t.lo <= kMaxUint63);
    return (uint128_t(t.hi) << 63) + t.lo;
}

auto from_uint128(uint128_t val) -> Int128
{
    assert(val <= kMaxUint126);
    auto result = Int128{
        .hi = uint64_t(val >> 63) & kMaxUint63,
        .lo = uint64_t(val) & kMaxUint63,
    };
    return result;
}


#if __EMSCRIPTEN__
using namespace emscripten;

auto uint128_to_val(uint128_t t) -> val
{
    auto x = from_uint128(t);
    auto v = val::object();
    v.set("hi", x.hi);
    v.set("lo", x.lo);
    return v;
}

auto val_to_uint128(const val& t) -> uint128_t
{
    auto hi = t["hi"].as<uint64_t>();
    auto lo = t["lo"].as<uint64_t>();
    assert(hi <= kMaxUint63);
    assert(lo <= kMaxUint63);
    return (uint128_t(hi) << 63) + uint64_t(lo);
}

EMSCRIPTEN_BINDINGS(Int128) {
    value_object<Int128>("Int128")
        .field("hi", &Int128::hi)
        .field("lo", &Int128::lo)
        ;

    function("to_uint128", &to_uint128);
    function("from_uint128", &from_uint128);

    function("uint128_to_val", &uint128_to_val);
    function("val_to_uint128", &val_to_uint128);
}
#endif



} // namespace pho::math
