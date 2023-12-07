#pragma once
#include <array>
#include <cstdint>
#include <functional>

namespace logstat {

using PropValue = uint32_t;

struct PropSet {
    static constexpr std::size_t kPropNumber = 10;
    std::array<PropValue, kPropNumber> props;
};

inline bool operator==(const PropSet & lhs, const PropSet & rhs)
{
    return lhs == rhs;
}

} // namespace logstat

template<>
struct std::hash<logstat::PropSet> {
    std::size_t operator()(const logstat::PropSet & propSet) const noexcept
    {
        size_t h = 0;
        // magic from stackoverflow, use boost combine instead
        for (auto e : propSet.props) {
            h ^= std::hash<logstat::PropValue>{}(e)  + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};
