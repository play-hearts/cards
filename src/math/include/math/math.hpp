#pragma once

#include <cstdint>
#include <string>

namespace pho::math {

using uint128_t = __uint128_t;

auto asDecimalString(uint128_t N) -> std::string;
// This is inefficient, but it's meant for unit tests and other performance non-critical code

auto asHexString(uint128_t N, unsigned zeroFillTo = 0) -> std::string;
// If the output would have less than zeroFileTo hex digits, pad on the left with zeros.
// This is inefficient, but it's meant for unit tests and other performance non-critical code

auto parseHex128(const std::string& hexString) -> uint128_t;

} // namespace pho::math
