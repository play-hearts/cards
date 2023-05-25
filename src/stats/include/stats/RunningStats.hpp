#pragma once
// stats/RunningStats.h
// Adapted from https://www.johndcook.com/blog/skewness_kurtosis/

#include <cstdint>
#include <ostream>
#include <string>

namespace pho::stats {

class RunningStats
{
public:
    RunningStats();
    ~RunningStats() = default;

    RunningStats(const RunningStats&) = default;
    RunningStats& operator=(const RunningStats&) = default;

    RunningStats(RunningStats&&) = default;
    RunningStats& operator=(RunningStats&&) = default;

    void clear();
    void accumulate(double x);
    void operator+=(double x) { accumulate(x); }

    // Count could be 32-bit but 64-bit is preferred
    using Count = uint64_t;

    Count size() const { return N; }
    Count samples() const { return size(); }

    double mean() const;
    double average() const { return mean(); }
    double variance() const;
    double stddev() const;
    double skewness() const;
    double kurtosis() const;

    double minimum() const { return mMin; }
    double maximum() const { return mMax; }

    friend RunningStats operator+(const RunningStats& a, const RunningStats& b);
    RunningStats& operator+=(const RunningStats &rhs);

    bool operator<(const RunningStats& other) const
    {
        return average() < other.average();
    }

    bool operator>(const RunningStats& other) const
    {
        return average() > other.average();
    }

    Count N;

private:
    double M1, M2, M3, M4;
    double mMin, mMax;
};

inline void clear(RunningStats& stats) { stats.clear(); }
inline void accumulate(RunningStats& stats, double sample) { stats.accumulate(sample); }

std::string toString(const RunningStats& accum);
std::ostream& operator<<(std::ostream&, const RunningStats& accum);

} // namespace pho::stats
