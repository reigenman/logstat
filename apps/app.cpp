#include <iostream>
#include <string_view>
#include <getopt.h>

// Use Boost.Program_options instead
struct ProgramOpts {
    std::string logDirPath;
    std::string logFilePrefix;
    std::string outFilePath;
    int threadNumber{1};

    void SetArg(const char * name, const char * value)
    {
        if (!value || !name) {
            badArgs_ = true;
            return;
        }
        std::string_view argName(name);
        if (argName == "logs_dir") {
            logDirPath = value;
        } else if (argName == "logs_prefix") {
            logFilePrefix = value;
        } else if (argName == "output") {
            outFilePath = value;
        } else {
            badArgs_ = true;
        }
    }

    void SetThreads(const char * value)
    {
        if (!value) {
            badArgs_ = true;
            return;
        }
        try {
            threadNumber = std::stoi(value);
        } catch (const std::exception & e) {
            badArgs_ = true;
        }

    }

    bool IsBadArgs() const { return badArgs_; }

    void SetBadArgs() { badArgs_ = true; }

    bool IsValid() const { return !IsBadArgs() && !logDirPath.empty() && !outFilePath.empty() && threadNumber > 0; }

    std::string GetUsage()
    {
        return "Usage: aggregator --logs_dir PATH --output FILE_PATH [--logs_prefix LOGS_FILES_PREFIX] [--threads THREAD_NUMBER]\n";
    }
private:
    bool badArgs_{false};
};

int main(int argc, char * argv[])
{
    ProgramOpts opts;

    option longOptions[] = {
        {"logs_dir", required_argument, 0, 0},
        {"logs_prefix", required_argument, 0, 0},
        {"output",  required_argument, 0, 0},
        {"threads",  required_argument, 0, 1},
        {"help",  no_argument, 0, 2},
        {0, 0, 0, 0}
    };
    bool failed = false;
    while (!opts.IsBadArgs()) {
        int optionIndex = 0;
        optarg = nullptr;
        int c = getopt_long(argc, argv, "", longOptions, &optionIndex);
        if (c == -1)
            break;
        switch (c) {
            case 0:
                opts.SetArg(longOptions[optionIndex].name, optarg);
                break;
            case 1:
                opts.SetThreads(optarg);
                break;
            case 2:
                std::cout << opts.GetUsage();
                return 0;
            default:
                opts.SetBadArgs();
                break;
        }
    }
    if (!opts.IsValid()) {
        std::cerr << "invalid parameters\n";
        std::cerr << opts.GetUsage();
        return 1;
    }

    return 0;
}
