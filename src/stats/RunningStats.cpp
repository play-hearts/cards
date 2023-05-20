// pho/stats/RunningStats.cpp
// Adapted from https://www.johndcook.com/blog/skewness_kurtosis/

#include "stats/RunningStats.hpp"

#include <cmath>
#include <fmt/format.h>
#include <sstream>

namespace pho::stats {

RunningStats::RunningStats()
: N{0}
, M1{0.0}
, M2{0.0}
, M3{0.0}
, M4{0.0}
, mMin{std::numeric_limits<double>::max()}
, mMax{std::numeric_limits<double>::lowest()}
{
}

void RunningStats::clear()
{
    *this = RunningStats{};
}

void RunningStats::accumulate(double x)
{
    N++;
    // Some intermediate expressions below can overflow if we use integer values for N-1 and N.
    const double n1 = N-1;
    const double n = N;

    const double delta = x - M1;
    const double delta_n = delta / N;
    const double delta_n2 = delta_n * delta_n;
    const double term1 = delta * delta_n * n1;

    M1 += delta_n;
    M4 += term1 * delta_n2 * (n*n - 3.0*n + 3.0) + 6.0 * delta_n2 * M2 - 4.0 * delta_n * M3;
    M3 += term1 * delta_n * (n - 2.0) - 3.0 * delta_n * M2;
    M2 += term1;

    mMin = std::min(mMin, x);
    mMax = std::max(mMax, x);
}

constexpr auto kNaN = std::numeric_limits<double>::quiet_NaN();

double RunningStats::mean() const
{
    return N==0 ? kNaN : M1;
}

double RunningStats::variance() const
{
    return N<=1 ? kNaN : M2/(N-1.0);
}

double RunningStats::stddev() const
{
    return std::sqrt(variance());
}

double RunningStats::skewness() const
{
    return N<=2 ? kNaN : std::sqrt(double(N)) * M3/ pow(M2, 1.5);
}

double RunningStats::kurtosis() const
{
    return N<=3 ? kNaN : N*M4 / (M2*M2) - 3.0;
}

RunningStats operator+(const RunningStats& a, const RunningStats& b)
{
    RunningStats combined;

    combined.N = a.N + b.N;

    // Some intermediate expressions below can overflow if we use integer values for a.N and b.N.
    const double aN = a.N;
    const double bN = b.N;
    const double cN = combined.N;

    const auto delta = b.M1 - a.M1;
    const auto delta2 = delta*delta;
    const auto delta3 = delta*delta2;
    const auto delta4 = delta2*delta2;

    combined.M1 = (aN*a.M1 + bN*b.M1) / cN;

    combined.M2 = a.M2 + b.M2 +
                  delta2 * aN * bN / cN;

    combined.M3 = a.M3 + b.M3 +
                  delta3 * aN * bN * (aN - bN)/(cN*cN);
    combined.M3 += 3.0*delta * (aN*b.M2 - bN*a.M2) / cN;

    combined.M4 = a.M4 + b.M4 + delta4*aN*bN * (aN*aN - aN*bN + bN*bN) /
                  (cN*cN*cN);
    combined.M4 += 6.0*delta2 * (aN*aN*b.M2 + bN*bN*a.M2)/(cN*cN) +
                  4.0*delta*(aN*b.M3 - bN*a.M3) / cN;

    combined.mMin = std::min(a.mMin, b.mMin);
    combined.mMax = std::max(a.mMax, b.mMax);

    return combined;
}

RunningStats& RunningStats::operator+=(const RunningStats& rhs)
{
    RunningStats combined = *this + rhs;
    *this = combined;
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const RunningStats& stats)
{
    if (stats.samples() == 0)
    {
        return stream << "No samples";
    }
    else
    {
        return stream << stats.mean() << "+/-" << stats.stddev()
            << " skew:" << stats.skewness() << " kurt:" <<  stats.kurtosis()
            << " min:" << stats.minimum() << " max:" <<  stats.maximum()
            << " N:" << stats.samples();
    }
}

std::string toString(const RunningStats& stats)
{
    const auto s = stats.stddev();
    const auto min = stats.minimum();
    const auto max = stats.maximum();
    return fmt::format("{:.2f}+/-{:.2f} [{:.2f}, {:.2f}] sk:{:.2f} kr:{:.2f} N:{}",
        stats.mean(), s, min, max, stats.skewness(), stats.kurtosis(), stats.samples()
    );
}

} // namespace pho::stats
