#include <iostream>

#include "WinUtil.h"
#include "Log.h"

namespace Spar
{

    std::vector<Log::LogMessageData> Log::m_messages;
    bool Log::m_initialized = false;
    
    void Log::Init()
    {
        m_initialized = true;
    }

    void Log::Shutdown()
    {
        m_initialized = false;
    }

    void Log::Info(const std::string &msg)
    {
        LogMessage(msg, LogLevel::Info);
    }

    void Log::Warn(const std::string &msg)
    {
        LogMessage(msg, LogLevel::Warn);
    }

    void Log::Error(const std::string &msg)
    {
        LogMessage(msg, LogLevel::Error);
    }

    void Log::LogMessage(const std::string &msg, LogLevel level)
    {
        if (!m_initialized)
        {
            return;
        }

        LogMessageData data;
        data.message = msg;
        data.level = level;

        m_messages.push_back(data);

        switch (level)
        {
        case LogLevel::Info:
            std::cout << "INFO: " << msg << std::endl;
            break;
        case LogLevel::Warn:
            std::cout << "WARN: " << msg << std::endl;
            break;
        case LogLevel::Error:
            std::cout << "ERROR: " << msg << std::endl;
            break;

        default:
            break;
        }
    }

};