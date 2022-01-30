#pragma once

#include <string>
#include <fmt/format.h>

namespace VALX
{
    class Logger
    {
    public:
        virtual void LogError(const std::string& who, const std::string& message) = 0;
        virtual void LogWarning(const std::string& who, const std::string& message) = 0;
        virtual void LogInfo(const std::string& who, const std::string& message) = 0;
        virtual ~Logger() = default;
    };

    class ConsoleLogger : public Logger
    {
    public:
        void LogError(const std::string& who, const std::string& message) override;
        void LogWarning(const std::string& who, const std::string& message)  override;
        void LogInfo(const std::string& who, const std::string& message) override;
    };

    Logger* SetCurrentLogger(Logger* logger);
    Logger* GetCurrentLogger();
}