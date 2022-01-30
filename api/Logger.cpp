#include "Logger.h"

#include <iostream>

namespace VALX
{
    static ConsoleLogger consoleLogger;

    static Logger* currentLogger = nullptr;

    Logger* SetCurrentLogger(Logger* logger)
    {
        Logger* oldLogger = currentLogger;
        currentLogger = logger;
        return oldLogger;
    }

    Logger* GetCurrentLogger()
    {
        return currentLogger ? currentLogger : &consoleLogger;
    }

    void ConsoleLogger::LogError(const std::string& who, const std::string& message)
    {
        std::cerr << fmt::format("[ERROR {}]: {}", who, message) << std::endl;
    }

    void ConsoleLogger::LogWarning(const std::string& who, const std::string& message)
    {
        std::cerr << fmt::format("[WARNING {}]: {}", who, message) << std::endl;
    }

    void ConsoleLogger::LogInfo(const std::string& who, const std::string& message)
    {
        std::cerr << fmt::format("[INFO {}]: {}", who, message) << std::endl;
    }
}