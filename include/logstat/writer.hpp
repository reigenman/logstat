#pragma once
#include "stats.hpp"
#include <memory>
#include <filesystem>

namespace logstat {

class Writer {
public:
    explicit Writer(const std::filesystem::path & outfilePath);
    ~Writer() noexcept;

    Writer(const Writer & ) = delete;
    Writer & operator=(const Writer & ) = delete;

    Writer(Writer && rhs) noexcept = default;
    Writer & operator=(Writer && rhs) noexcept = default;

    void WriteResult(DayCountMap && dayStat);

private:
    struct Private;
    std::unique_ptr<Private> p_;
};

} // namespace logstat
