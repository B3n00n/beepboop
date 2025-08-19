#include "pch.h"
#include "KeyboardMonitor.h"
#include "../Utils/Logger.h"

namespace B3n00n
{
    KeyboardMonitor* KeyboardMonitor::s_instance = nullptr;

    KeyboardMonitor::KeyboardMonitor() : m_keyboardHook(nullptr), m_isRunning(false)
    {
        s_instance = this;
    }

    KeyboardMonitor::~KeyboardMonitor()
    {
        Stop();
        s_instance = nullptr;
    }

    void KeyboardMonitor::SetInputHandler(std::shared_ptr<IInputHandler> handler)
    {
        m_handler = handler;
    }

    void KeyboardMonitor::Start()
    {
        if (m_isRunning) return;

        m_isRunning = true;
        m_monitorThread = std::thread(&KeyboardMonitor::MonitorLoop, this);
    }

    void KeyboardMonitor::Stop()
    {
        m_isRunning = false;

        if (m_keyboardHook)
        {
            PostThreadMessage(GetThreadId(m_monitorThread.native_handle()), WM_QUIT, 0, 0);
        }

        if (m_monitorThread.joinable())
        {
            m_monitorThread.join();
        }
    }

    void KeyboardMonitor::MonitorLoop()
    {
        m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);

        if (!m_keyboardHook)
        {
            return;
        }

        MSG msg;
        while (m_isRunning && GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = nullptr;
    }

    LRESULT CALLBACK KeyboardMonitor::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode >= 0 && s_instance && s_instance->m_handler)
        {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

            if (wParam == WM_KEYDOWN)
            {
                s_instance->m_handler->OnKeyPress(kbStruct->vkCode);
            }
            else if (wParam == WM_KEYUP)
            {
                s_instance->m_handler->OnKeyRelease(kbStruct->vkCode);
            }
        }

        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
}