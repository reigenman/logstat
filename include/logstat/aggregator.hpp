#pragma once
#include "stats.hpp"
#include <string_view>

namespace logstat {

class Aggregator {
public:

    void AddLine(std::string_view line);
    unsigned GetSkippedLinesCount() const { return skippedLines_; }

    DayCountMap GetResult() &&;
private:
    void LogError();

    DayCountMap stats_;
    unsigned skippedLines_{0};
};

} // namespace logstat
