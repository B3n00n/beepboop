#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <sstream>

namespace B3n00n
{
    struct FrequencySignature;

    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger
    {
    public:
        static Logger& Instance();

        void Log(LogLevel level, const std::string& message);
        void LogBeepDetection(const FrequencySignature& sig, bool inVehicle);
        bool IsConsoleInitialized() const { return m_consoleInitialized; }

        template<typename... Args>
        void LogFormat(LogLevel level, const std::string& format, Args... args)
        {
            char buffer[512];
            snprintf(buffer, sizeof(buffer), format.c_str(), args...);
            Log(level, std::string(buffer));
        }

    private:
        Logger();
        ~Logger();

        void InitializeConsole();
        std::string GetTimestamp() const;
        std::string LevelToString(LogLevel level) const;

        std::mutex m_mutex;
        bool m_consoleInitialized;
    };

#define LOG_DEBUG(msg) B3n00n::Logger::Instance().Log(B3n00n::LogLevel::Debug, msg)
#define LOG_INFO(msg) B3n00n::Logger::Instance().Log(B3n00n::LogLevel::Info, msg)
#define LOG_WARNING(msg) B3n00n::Logger::Instance().Log(B3n00n::LogLevel::Warning, msg)
#define LOG_ERROR(msg) B3n00n::Logger::Instance().Log(B3n00n::LogLevel::Error, msg)
}