#pragma once
#include <string>
#include <unordered_map>

namespace logstat {

using PropsCountMap = std::unordered_map<std::string, unsigned>;

using FactCountMap = std::unordered_map<std::string, PropsCountMap>;

using DayCountMap = std::unordered_map<uint32_t, FactCountMap>;

}
