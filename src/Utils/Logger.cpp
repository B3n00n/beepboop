#include "pch.h"
#include "Logger.h"
#include "../Detection/FrequencySignature.h"
#include <iomanip>
#include <ctime>

namespace B3n00n
{
    Logger& Logger::Instance()
    {
        static Logger instance;
        return instance;
    }

    Logger::Logger() : m_consoleInitialized(false)
    {
        InitializeConsole();
    }

    Logger::~Logger()
    {
        if (m_consoleInitialized)
        {
            FreeConsole();
        }
    }

    void Logger::InitializeConsole()
    {
        if (AllocConsole())
        {
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            m_consoleInitialized = true;

            SetConsoleTitleA("B3n00n - Beep Detector");
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);

            std::cout << R"(
  ____    _____  _   _  ___    ___   _   _ 
 | __ )  |___ / | \ | |/ _ \  / _ \ | \ | |
 |  _ \    |_ \ |  \| | | | || | | ||  \| |
 | |_) |  ___) || |\  | |_| || |_| || |\  |
 |____/  |____/ |_| \_|\___/  \___/ |_| \_|
)" << std::endl;

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

            std::cout << "[+] Listening for boop beep beep..." << std::endl;
        }
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (level < LogLevel::Error) return;

        std::string timestamp = GetTimestamp();
        std::string levelStr = LevelToString(level);

        if (m_consoleInitialized)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "[-] ERROR: " << message << std::endl;

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        std::string debugMsg = "[LOG] [" + levelStr + "] " + message + "\n";
        OutputDebugStringA(debugMsg.c_str());
    }

    void Logger::LogBeepDetection(const FrequencySignature& sig, bool inVehicle)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_consoleInitialized)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            if (inVehicle)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                std::cout << "[+] BEEP BOOP DETECTED IN VEHICLE" << std::endl;
                OutputDebugStringA("[+] BEEP BOOP DETECTED IN VEHICLE!\n");
            }
            else
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                std::cout << "[-] BEEP BOOP DETECTED OUTSIDE OF VEHICLE" << std::endl;
                OutputDebugStringA("[-] BEEP BOOP DETECTED OUTSIDE OF VEHICLE!\n");
            }

            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }

    std::string Logger::GetTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        struct tm timeinfo;
        localtime_s(&timeinfo, &time_t);
        ss << std::put_time(&timeinfo, "%H:%M:%S");
        return ss.str();
    }

    std::string Logger::LevelToString(LogLevel level) const
    {
        switch (level)
        {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        default: return "UNKNOWN";
        }
    }
}