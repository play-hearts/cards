#pragma once

#include <cstdint>

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif


namespace pho::math {

typedef __uint128_t uint128_t;

struct Int128
{
    // Javascript does not support u64, but we fake it
    uint64_t hi;
    uint64_t lo;
};

auto to_uint128(const Int128& t) -> uint128_t;
auto from_uint128(uint128_t val) -> Int128;

#if __EMSCRIPTEN__
auto uint128_to_val(uint128_t) -> emscripten::val;
auto val_to_uint128(const emscripten::val& t) -> uint128_t;
// auto add_uint128(const emscripten::val& a, const emscripten::val& b) -> emscripten::val;
#endif

} // namespace pho::math
