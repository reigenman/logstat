#pragma once
#include <stdexcept>

namespace logstat {

struct ParseError : public std::runtime_error {
    explicit ParseError(const char * what) : std::runtime_error(what) {}
    explicit ParseError(const std::string & what) : std::runtime_error(what) {}
};

} // namespace logstat
