#pragma once
#include <vector>
#include <complex>

namespace B3n00n
{
    struct FrequencySpectrum
    {
        std::vector<float> magnitudes;
        std::vector<float> frequencies;
        float sampleRate;
    };

    class IFrequencyAnalyzer
    {
    public:
        virtual ~IFrequencyAnalyzer() = default;
        virtual FrequencySpectrum Analyze(const std::vector<float>& samples) = 0;
    };
}