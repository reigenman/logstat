#include <logstat/aggregator.hpp>
#include <logstat/prop_set.hpp>
#include <map>
#include "jsonlib.hpp"


namespace logstat {

namespace rj = rapidjson;

namespace {
constexpr std::string_view kTsFact("ts_fact");
constexpr std::string_view kFactName("fact_name");
constexpr std::string_view kProps("props");

constexpr unsigned kSecInDay = 60 * 60 * 24;

 auto MakePropToIdxMap() {
    std::map<std::string, size_t> propToIdx;
    for (size_t i = 0; i < PropSet::kPropNumber; ++i) {
        propToIdx["prop" + std::to_string(i + 1)] = i;
    }
    return propToIdx;
}
const auto & PropToIdxMap() {
    static const std::map<std::string, size_t> propToIdx = MakePropToIdxMap();
    return propToIdx;
}
std::string CollectProps(const rj::Document::Object & props)
{
    std::string result;
    PropSet propValues;
    const auto & propToIdx = PropToIdxMap();
    for (const auto & prop : props) {
        if (!prop.value.IsUint()) {
            return result;
        }
        auto propIt = propToIdx.find(prop.name.GetString());
        if (propIt == propToIdx.end()) {
            return result;
        }
        propValues.props[propIt->second] = prop.value.GetUint();
    }
    auto lastValIt = std::prev(propValues.props.end());
    for (auto it = propValues.props.begin(); it != lastValIt; ++it) {
        result.append(std::to_string(*it) + ",");
    }
    result.append(std::to_string(*lastValIt));
    return result;
}

} // namespace

void logstat::Aggregator::AddLine(std::string_view line)
{
    rj::Document document;
    rj::ParseResult parseRes = document.Parse(line.data(), line.size());
    if (!parseRes || !document.IsObject()) {
        LogError();
        return;
    }
    uint32_t tsFact = std::numeric_limits<uint32_t>::max();
    const char * factName = nullptr;
    auto propNodeIt = document.MemberEnd();
    for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
        const char * memberName = it->name.GetString();
        const auto & value = it->value;
        if (memberName == kTsFact) {
            if (!value.IsUint()) {
                break;
            }
            tsFact = value.GetUint();
        } else if (memberName == kFactName) {
            if (!value.IsString()) {
                break;
            }
            factName = value.GetString();
        } else if (memberName == kProps) {
            if (!value.IsObject()) {
                break;
            }
            propNodeIt = it;
        }
    }
    if (tsFact == std::numeric_limits<uint32_t>::max() || !factName || propNodeIt == document.MemberEnd()) {
        LogError();
        return;
    }
    const auto & props = propNodeIt->value.GetObject();
    if (props.MemberCount() != PropSet::kPropNumber) {
        LogError();
        return;
    }
    uint32_t dayTime = (tsFact / kSecInDay) * kSecInDay;
    auto propComb = CollectProps(props);
    if (propComb.empty()) {
        LogError();
        return;
    }

    stats_[dayTime][factName][std::move(propComb)] += 1;
}

DayCountMap Aggregator::GetResult() &&
{
    return std::move(stats_);
}

void Aggregator::LogError()
{
    skippedLines_++;
}

} // namespace logstat

