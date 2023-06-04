#pragma once

#include <cstdint>

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif


namespace pho::math {

typedef __uint128_t uint128_t;

struct Int126
{
    // Javascript does not support u64. So we effectively use two u63s instead.
    uint64_t hi;
    uint64_t lo;
};

auto to_uint128(const Int126& t) -> uint128_t;
auto from_uint128(uint128_t val) -> Int126;

#if __EMSCRIPTEN__
auto uint128_to_val(uint128_t) -> emscripten::val;
auto val_to_uint128(const emscripten::val& t) -> uint128_t;
#endif

} // namespace pho::math
