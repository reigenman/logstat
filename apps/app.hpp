#pragma once
#include <string>
#include <memory>
#include <filesystem>

namespace logstat::app {

namespace fs = std::filesystem;

class Application {
public:
    explicit Application(unsigned threadsNum);
    ~Application() noexcept;
    Application(const Application &) = delete;
    Application & operator=(const Application &) = delete;
    Application(Application &&) noexcept = default;
    Application & operator=(Application &&) = delete;

    void Run(const fs::path & logDir, const fs::path & outFile, const std::string & logFilePrefix);
private:
    struct Private;
    std::unique_ptr<Private> p_;
};
}
