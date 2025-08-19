#pragma once

namespace B3n00n
{
    class IInputHandler
    {
    public:
        virtual ~IInputHandler() = default;
        virtual void OnKeyPress(int virtualKey) = 0;
        virtual void OnKeyRelease(int virtualKey) = 0;
    };
}