#pragma once
#include "stats.hpp"
#include <string_view>

namespace logstat {


class Aggregator {
public:

    void AddLine(std::string_view line);

private:
    void LogError();
    DayCountMap stats_;
    unsigned skippedLines_{0};
};
}
