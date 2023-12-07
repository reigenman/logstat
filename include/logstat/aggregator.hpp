#pragma once
#include "stats.hpp"
#include <string_view>
#include <stdexcept>

namespace logstat {

struct ParseError : public std::runtime_error {
    ParseError(const char * what) : std::runtime_error(what) {}
};

class Aggregator {
public:

    void AddLine(std::string_view line);

private:
    void LogError();
    DayCountMap stats_;
    unsigned skippedLines_{0};
};
}
