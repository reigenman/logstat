#include "app.hpp"
#include <fstream>
#include <iostream>
#include <logstat/aggregator.hpp>
#include <logstat/writer.hpp>

namespace logstat::app {

namespace {

bool FilterLogName(const fs::path & path, const std::string & logFilePrefix)
{
    if (logFilePrefix.empty()) {
        return true;
    }
    return path.filename().string().find(logFilePrefix) != std::string::npos;
}

void CollectStatForFile(const fs::path & path, logstat::DayCountMap & result)
{
    std::ifstream inFile(path);
    std::string line;
    logstat::Aggregator aggregator;
    while (std::getline(inFile, line)) {
        aggregator.AddLine(line);
    }
    result = std::move(aggregator).GetResult();
}

} // namespace

struct Application::Private {
    explicit Private(unsigned int threadsNum) {}


    void Run(const fs::path & logDir, const fs::path & outFile, const std::string & logFilePrefix)
    {
        Writer writer(outFile);
        for (const auto & dirEntry : std::filesystem::directory_iterator(logDir)) {
            if (dirEntry.is_regular_file() && FilterLogName(dirEntry.path(), logFilePrefix)) {
                logstat::DayCountMap result;
                CollectStatForFile(dirEntry.path(), result);
                writer.WriteResult(std::move(result));
            }
        }
    }
};

Application::Application(unsigned int threadsNum)
    : p_(new Private(threadsNum))
{}
Application::~Application() noexcept
{}

void Application::Run(const fs::path & logDir, const fs::path & outFile, const std::string & logFilePrefix)
{
    p_->Run(logDir, outFile, logFilePrefix);
}

}
