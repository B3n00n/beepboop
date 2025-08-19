#pragma once
#include "../Detection/FrequencySignature.h"
#include <functional>

namespace B3n00n
{
    class BeepDetector
    {
    public:
        using DetectionCallback = std::function<void(const FrequencySignature&)>;

        BeepDetector();

        void SetDetectionCallback(DetectionCallback callback);
        bool IsBeepSound(const FrequencySignature& sig) const;

    private:
        DetectionCallback m_callback;

        bool CheckDuration(float duration) const;
        bool CheckFundamentalFrequency(float freq) const;
        bool CheckSpectralProperties(float centroid, float spread) const;
        bool CheckPeakFrequencies(const std::vector<float>& peaks) const;
    };
}