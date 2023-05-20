#pragma once

#include "prim/range.hpp"

#include <vector>
#include <functional>
#include <cmath>

namespace pho::stats {

constexpr auto UNEXPECTED = false;

enum AxisType
{
    eLinear,
    eLog,
    eLogLog,
    eExp,
    eExpExp
};

using Transform = std::function<double(double)>;

namespace detail {

double identity(double x) { return x; }
double log(double x) { return std::log2(x); }
double loglog(double x) { return log(log(x)); }
double exp(double x) { return std::exp2(x); }
double expexp(double x) { return exp(exp(x)); }


Transform forward(AxisType type)
{
    switch (type)
    {
        case eLinear: return identity;
        case eLog: return log;
        case eLogLog: return loglog;
        case eExp: return exp;
        case eExpExp: return expexp;
        default:
            assert(UNEXPECTED);
            return identity;
    }
}

Transform inverse(AxisType type)
{
    switch (type)
    {
        case eLinear: return identity;
        case eLog: return exp;
        case eLogLog: return expexp;
        case eExp: return log;
        case eExpExp: return loglog;
        default:
            assert(UNEXPECTED);
            return identity;
    }
}

} // namespace detail

class Parameters
{
public:
    Parameters(double alpha, double beta, AxisType xAxis=eLinear, AxisType yAxis=eLinear)
    : mAlpha(alpha)
    , mBeta(beta)
    , xAxis(xAxis)
    , yAxis(yAxis)
    {}

    using X_t = double;
    using Y_t = double;

    Y_t yFromX(X_t x) const
    {
        return detail::inverse(yAxis)(detail::forward(xAxis)(x)*mBeta + mAlpha);
    }

    X_t xFromY(Y_t y) const
    {
        return detail::inverse(xAxis)((detail::forward(yAxis)(y) - mAlpha) / mBeta);
    }

    double alpha() const { return mAlpha; }
    double beta() const { return mBeta; }

private:
    double mAlpha;
    double mBeta;
    AxisType xAxis;
    AxisType yAxis;
};


class LineOfBestFit
{
public:
    using XData = std::vector<double>;
    using YData = std::vector<double>;

    LineOfBestFit(const XData&  x, const YData&  y)
    : mSize(x.size())
    , xData(x)
    , yData(y)
    {
        assert(mSize == y.size());
    }

    Parameters parametersFor(AxisType xAxis=eLinear, AxisType yAxis=eLinear)
    {
        double X1{};
        double Y1{};

        for (auto i : prim::range(mSize))
        {
            auto x = detail::forward(xAxis)(xData.at(i));
            auto y = detail::forward(yAxis)(yData.at(i));
            X1 += x;
            Y1 += y;
        }

        const double kScale = 1.0 / mSize;
        X1 *= kScale;
        Y1 *= kScale;

        double num{};
        double den{};
        for (auto i : prim::range(mSize))
        {
            const auto x = detail::forward(xAxis)(xData.at(i)) - X1;
            num += x * (detail::forward(yAxis)(yData.at(i))-Y1);
            den += x * x;
        }

        double beta = num / den;
        double alpha = Y1 - beta * X1;
        Parameters params{ alpha, beta, xAxis, yAxis };
        return params;
    }

    double quality(const Parameters& params)
    {
        YData yPred{};

        double yMean{};
        for (const auto y : yData)
        {
            yMean += y;
        }
        yMean /= mSize;

        double SStot{};
        double SSres{};
        for (const auto i : prim::range(mSize))
        {
            const auto x = xData.at(i);
            const auto yTrue = yData.at(i);
            const auto yPred = params.yFromX(x);

            SStot += (yTrue - yMean)*(yTrue - yMean);
            SSres += (yTrue - yPred)*(yTrue - yPred);
        }
        return 1.0 - SSres/SStot;
    }

private:
    size_t mSize;
    XData xData;
    YData yData;
};

} // namespace pho::stats
