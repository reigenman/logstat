#include "app.hpp"
#include <fstream>
#include <iostream>
#include <logstat/aggregator.hpp>

namespace logstat::app {

namespace {

void CreateFile(const fs::path & path)
{
    std::ofstream ofs(path,  std::ios::out | std::ios::trunc);
    if (!ofs) {
        throw std::runtime_error("failed to open the file: " + path.string());
    }
}

bool FilterLogName(const fs::path & path, const std::string & logFilePrefix)
{
    if (logFilePrefix.empty()) {
        return true;
    }
    return path.filename().string().find(logFilePrefix) != std::string::npos;
}

void CollectStatForFile(const fs::path & path)
{
    std::ifstream inFile(path);
    std::string line;
    logstat::Aggregator aggregator;
    while (std::getline(inFile, line)) {
        aggregator.AddLine(line);
    }
}

} // namespace

struct Application::Private {
    explicit Private(unsigned int threadsNum) {}


    void Run(const fs::path & logDir, const fs::path & outFile, const std::string & logFilePrefix)
    {
        CreateFile(outFile);
        for (const auto & dirEntry : std::filesystem::directory_iterator(logDir)) {
            if (dirEntry.is_regular_file() && FilterLogName(dirEntry.path(), logFilePrefix)) {
                CollectStatForFile(dirEntry.path());
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
