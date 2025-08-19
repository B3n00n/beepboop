#pragma once
#include <vector>

namespace B3n00n {
    class IAudioProcessor
    {
    public:
        virtual ~IAudioProcessor() = default;
        virtual void ProcessBuffer(const float* samples, size_t sampleCount) = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual bool IsRunning() const = 0;
    };
}