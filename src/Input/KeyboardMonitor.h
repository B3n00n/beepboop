#pragma once
#include "IInputHandler.h"
#include <windows.h>
#include <thread>
#include <atomic>

namespace B3n00n
{
    class KeyboardMonitor
    {
    public:
        KeyboardMonitor();
        ~KeyboardMonitor();

        void SetInputHandler(std::shared_ptr<IInputHandler> handler);
        void Start();
        void Stop();

    private:
        static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

        std::shared_ptr<IInputHandler> m_handler;
        std::thread m_monitorThread;
        std::atomic<bool> m_isRunning;
        HHOOK m_keyboardHook;

        void MonitorLoop();
        static KeyboardMonitor* s_instance;
    };
}