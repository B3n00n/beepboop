#pragma once
#include <vector>

namespace B3n00n
{
    struct FrequencySignature
    {
        std::vector<float> peakFrequencies;
        std::vector<float> peakAmplitudes;
        float fundamentalFreq;
        float spectralCentroid;
        float spectralSpread;
        float duration;

        FrequencySignature() : fundamentalFreq(0), spectralCentroid(0), spectralSpread(0), duration(0) {}
    };
}