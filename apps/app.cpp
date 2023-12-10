#include "app.hpp"
#include <fstream>
#include <iostream>
#include <logstat/aggregator.hpp>
#include <logstat/writer.hpp>
#include <logstat/thread_pool.hpp>
#include <logstat/threadsafe_queue.hpp>
#include <chrono>

namespace logstat::app {

namespace {

using ResultQueue = Queue<DayCountMap>;
using ResultQueuePtr = std::shared_ptr<ResultQueue>;

bool FilterLogName(const fs::path & path, const std::string & logFilePrefix)
{
    if (logFilePrefix.empty()) {
        return true;
    }
    return path.filename().string().find(logFilePrefix) == 0u;
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

void AsyncCollectStatForFile(const fs::path & path, ResultQueuePtr resultQueue)
{
    logstat::DayCountMap result;
    try {
        auto statTp = std::chrono::steady_clock::now();
        CollectStatForFile(path, result);
        std::chrono::duration<double> procTime = std::chrono::steady_clock::now() - statTp;
        std::cout << "file=" << path << ", proc_time="  << procTime.count() << " secs\n";
    } catch (const std::exception & e) {
        std::cerr << "caught exception while collection: " << e.what() << "\n";
    }
    resultQueue->Push(std::move(result));
}

} // namespace

struct Application::Private {
    explicit Private(unsigned int threadsNum)
        : thPool(threadsNum)
          , threadNumber(threadsNum)
    {}


    void Run(const fs::path & logDir, const fs::path & outFile, const std::string & logFilePrefix)
    {
        Writer writer(outFile);
        auto resultQueue = std::make_shared<ResultQueue>();
        int filesCount = 0;
        for (const auto & dirEntry : std::filesystem::directory_iterator(logDir)) {
            if (dirEntry.is_regular_file() && FilterLogName(dirEntry.path(), logFilePrefix)) {
                thPool.Post([path = dirEntry.path(), results = resultQueue] {
                    AsyncCollectStatForFile(path, results);
                });
                ++filesCount;

                // stop pushing task to avoid many result in the queue
                // if writing is too slow
                if (filesCount >= threadNumber) {
                    DayCountMap result;
                    resultQueue->Get(result);
                    writer.WriteResult(std::move(result));
                    --filesCount;
                }
            }
        }
        // consume rest results
        for (int i = 0; i < filesCount; ++i) {
            DayCountMap result;
            resultQueue->Get(result);
            writer.WriteResult(std::move(result));
        }

    }
    ThreadPool thPool;
    unsigned threadNumber;
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
