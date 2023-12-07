#include <logstat/aggregator.hpp>
#include <map>

#define RAPIDJSON_ASSERT(x) (!(x) ? throw logstat::ParseError("broken line") : (void)0u)

#include <rapidjson/document.h>

namespace logstat {

namespace rj = rapidjson;

namespace {
constexpr char kTsFact[] = "ts_fact";
constexpr char kFactName[] = "fact_name";
constexpr char kProps[] = "props";

constexpr unsigned kSecInDay = 60 * 60 * 24;

 auto MakePropToIdxMap() {
    std::map<std::string, size_t> propToIdx;
    for (size_t i = 0; i < PropSet::kPropNumber; ++i) {
        propToIdx["prop" + std::to_string(i)] = i;
    }
    return propToIdx;
}

const auto & PropToIdxMap() {
    static const std::map<std::string, size_t> propToIdx = MakePropToIdxMap();
    return propToIdx;
}

} // namespace

void logstat::Aggregator::AddLine(std::string_view line)
{
    rj::Document document;
    document.Parse(line.data(), line.size());
    const auto & tsFactNode = document[kTsFact];
    const auto & factNameNode = document[kFactName];
    const auto & propsNode = document[kProps];
    if (!tsFactNode.IsUint() || factNameNode.IsString() || !propsNode.IsObject()) {
        LogError();
        return;
    }
    const auto & props = propsNode.GetObject();
    if (props.MemberCount() != PropSet::kPropNumber) {
        LogError();
        return;
    }
    PropSet propSet;
    const auto & idxMap = PropToIdxMap();
    for (const auto & prop : props) {
        if (!prop.value.IsUint()) {
            LogError();
            return;
        }
        auto idxIt = idxMap.find(prop.name.GetString());
        if (idxIt == idxMap.cend()) {
            LogError();
            return;
        }
        propSet.props[idxIt->second] = prop.value.GetUint();
    }
    uint32_t dayTime = tsFactNode.GetUint() / kSecInDay;
    const auto * factName = factNameNode.GetString();

    stats_[dayTime][factName][propSet] += 1;
}

void Aggregator::LogError()
{
    skippedLines_++;
}

} // namespace logstat

