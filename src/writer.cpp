#include <logstat/writer.hpp>
#include <stdexcept>
#include <fstream>
#include "jsonlib.hpp"
#include <time.h>

namespace logstat {


struct Writer::Private {
    explicit Private(const std::filesystem::path & outfilePath)
        : file(outfilePath,  std::ios::out | std::ios::trunc)
        , wrapper(file)
        , writer(wrapper)
    {
        if (!file) {
            throw std::runtime_error("failed to open the file: " + outfilePath.string());
        }
        writer.StartObject();
    }
    ~Private() noexcept
    {
        try {
            writer.EndObject();
        } catch (...) {
        }
    }

    void WriteResult(DayCountMap && dayStat)
    {
        for (auto & [day, facts] : dayStat) {
            tm tb;
            time_t tsec = static_cast<time_t>(day);
            tm * gmTime = gmtime_r(&tsec, &tb);
            if (!gmTime) {
                continue;
            }
            char timeString[std::size("yyyy-mm-dd")];
            std::strftime(std::data(timeString), std::size(timeString), "%Y-%m-%d", gmTime);
            // paranoic check
            timeString[std::size(timeString) - 1] = '\0';
            writer.Key(timeString);
            writer.StartObject();
            for (auto & [fact, props] : facts) {
                writer.Key(fact.c_str());
                writer.StartObject();
                for (auto & [props, number] : props) {
                    writer.Key(props.c_str());
                    writer.Uint(number);
                }
                writer.EndObject();
            }
            writer.EndObject();
        }
    }
    std::ofstream file;
    rapidjson::OStreamWrapper wrapper;
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer;
};

Writer::Writer(const std::filesystem::path & outfilePath)
    : p_(new Private(outfilePath))
{}

void Writer::WriteResult(DayCountMap && dayStat)
{
    p_->WriteResult(std::move(dayStat));
}

Writer::~Writer() noexcept
{}

} // namespace logstat
