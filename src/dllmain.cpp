#include "pch.h"
#include "Core/AudioAnalyzer.h"
#include "Utils/Logger.h"
#include <memory>
#include <thread>

namespace B3n00n
{
    std::unique_ptr<AudioAnalyzer> g_analyzer;
    std::thread g_analyzerThread;

    void AnalyzerThreadProc()
    {
        try {
            Logger::Instance();

            g_analyzer = std::make_unique<AudioAnalyzer>();

            if (g_analyzer->Initialize())
            {
                g_analyzer->Start();

                while (g_analyzer->IsRunning())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
        catch (const std::exception& e)
        {
            // Silent fail
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        B3n00n::g_analyzerThread = std::thread(B3n00n::AnalyzerThreadProc);
        break;

    case DLL_PROCESS_DETACH:
        if (B3n00n::g_analyzer)
        {
            B3n00n::g_analyzer->Stop();
            B3n00n::g_analyzer.reset();
        }

        if (B3n00n::g_analyzerThread.joinable())
        {
            B3n00n::g_analyzerThread.join();
        }
        break;
    }
    return TRUE;
}