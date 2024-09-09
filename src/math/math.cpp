#include "math/math.hpp"
#include <assert.h>
#include <ctype.h>

namespace pho::math {

auto asDecimalString(uint128_t N) -> std::string
{
    if (N == 0)
        return "0";

    std::string result;
    while (N > 0)
    {
        unsigned d = (N % 10);
        N /= 10;
        result = std::string(1, char('0' + d)) + result;
    }

    return result;
}

auto asHexString(uint128_t N, unsigned zeroFillTo) -> std::string
{
    if (N == 0)
        return "0";

    std::string result;
    while (N > 0)
    {
        unsigned d = (N % 16);
        N /= 16;
        char c = d < 10 ? char('0' + d) : char('a' + d - 10);
        result = std::string(1, c) + result;
    }

    if (result.length() < zeroFillTo)
    {
        result = std::string(zeroFillTo - result.length(), '0') + result;
    }

    return result;
}

auto parseHex128(const std::string& str) -> uint128_t
{
    const char* line = str.c_str();
    uint128_t N = 0;
    for (; isxdigit(*line); ++line)
    {
        const char c = *line;
        if (c >= '0' && c <= '9')
        {
            N *= 16;
            N += (c - '0');
        }
        else if (c >= 'a' && c <= 'f')
        {
            N *= 16;
            N += (c - 'a' + 10);
        }
        else if (c >= 'A' && c <= 'F')
        {
            N *= 16;
            N += (c - 'A' + 10);
        }
        else
        {
            assert(false);
        }
    }
    return N;
}

} // namespace pho::math
