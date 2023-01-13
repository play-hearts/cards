// prim/dlog.h

#pragma once

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fmt/printf.h>

class DLog
{
public:
    DLog(const std::string& channel, FILE* outFile=stderr)
    : mChannel(channel)
    , mOutFile(outFile)
    , mOn(enabled())
    , mColorize(isatty(fileno(outFile)))
    {}

    template <typename ...T>
    void operator()(fmt::format_string<T...> fmt, T&&... args) const
    {
        if (mOn)
        {
            const auto kColorPrefix = mColorize ? "\033[31m" : "";
            const auto kColorSuffix = mColorize ? "\033[0m" : "";
            const auto x = fmt::format(fmt, std::forward<T>(args)...);
            fmt::print(mOutFile, "{}{}{}: {}\n", kColorPrefix, mChannel, kColorSuffix, x);
            fflush(mOutFile);
        }
    }

private:
    bool enabled()
    {
        const char* channels = ::getenv("DEBUG");
        return channels != nullptr && std::string{channels}.find(mChannel) != std::string::npos;
    }

private:
    std::string mChannel;
    FILE* mOutFile;
    bool mOn;
    bool mColorize;
};
