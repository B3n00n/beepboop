#pragma once
#include "../Core/IFrequencyAnalyzer.h"
#include "../Detection/FrequencySignature.h"

namespace B3n00n
{
    class FrequencyAnalyzer : public IFrequencyAnalyzer
    {
    public:
        FrequencyAnalyzer(float sampleRate, int fftSize);

        FrequencySpectrum Analyze(const std::vector<float>& samples) override;
        FrequencySignature AnalyzeSignature(const std::vector<float>& buffer, float duration);

    private:
        float m_sampleRate;
        int m_fftSize;

        struct Peak {
            float frequency;
            float amplitude;
        };

        std::vector<Peak> FindPeaks(const std::vector<float>& magnitude);
        float CalculateSpectralCentroid(const std::vector<float>& magnitude);
        float CalculateSpectralSpread(const std::vector<float>& magnitude, float centroid);
    };
}