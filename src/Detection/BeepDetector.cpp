#include "pch.h"
#include "BeepDetector.h"
#include "../Utils/Constants.h"
#include <algorithm>

namespace B3n00n {
    BeepDetector::BeepDetector()
    {
    }

    void BeepDetector::SetDetectionCallback(DetectionCallback callback)
    {
        m_callback = callback;
    }

    bool BeepDetector::IsBeepSound(const FrequencySignature& sig) const
    {
        if (!CheckDuration(sig.duration)) return false;
        if (!CheckFundamentalFrequency(sig.fundamentalFreq)) return false;
        if (!CheckSpectralProperties(sig.spectralCentroid, sig.spectralSpread)) return false;
        if (!CheckPeakFrequencies(sig.peakFrequencies)) return false;

        if (m_callback)
        {
            m_callback(sig);
        }

        return true;
    }

    bool BeepDetector::CheckDuration(float duration) const
    {
        return duration >= Constants::MIN_BEEP_DURATION_MS &&
            duration <= Constants::MAX_BEEP_DURATION_MS;
    }

    bool BeepDetector::CheckFundamentalFrequency(float freq) const
    {
        return (freq >= Constants::BEEP_FREQ1_MIN && freq <= Constants::BEEP_FREQ1_MAX);
    }

    bool BeepDetector::CheckSpectralProperties(float centroid, float spread) const
    {
        return centroid >= Constants::CENTROID_MIN && centroid <= Constants::CENTROID_MAX && spread >= Constants::SPREAD_MIN && spread <= Constants::SPREAD_MAX;
    }

    bool BeepDetector::CheckPeakFrequencies(const std::vector<float>& peaks) const
    {
        bool hasPrimaryFreq = false;
        bool hasSecondaryFreq = false;

        for (float freq : peaks) {
            if (freq >= Constants::BEEP_FREQ1_MIN && freq <= Constants::BEEP_FREQ1_MAX)
            {
                hasPrimaryFreq = true;
            }
            if (freq >= Constants::BEEP_FREQ2_MIN && freq <= Constants::BEEP_FREQ2_MAX)
            {
                hasSecondaryFreq = true;
            }
        }

        return hasPrimaryFreq;
    }
}