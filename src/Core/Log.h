#pragma once

#include <string>
#include <vector>

namespace Spar
{
    class Log
    {
    public:
        static void Init();
        static void Shutdown();
        static void Info(const std::string &msg);
        static void Warn(const std::string &msg);
        static void Error(const std::string &msg);

    private:
        enum class LogLevel
        {
            Info,
            Warn,
            Error
        };

        static void LogMessage(const std::string &msg, LogLevel level);

        struct LogMessageData
        {
            std::string message;
            LogLevel level;
        };

    private:
        static std::vector<LogMessageData> m_messages;
        static bool m_initialized;
    };

};
