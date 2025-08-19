#include "pch.h"
#include "FrequencyAnalyzer.h"
#include "FFTProcessor.h"
#include "../Utils/Constants.h"
#include <algorithm>

namespace B3n00n
{
    FrequencyAnalyzer::FrequencyAnalyzer(float sampleRate, int fftSize) : m_sampleRate(sampleRate), m_fftSize(fftSize)
    {
    }

    FrequencySpectrum FrequencyAnalyzer::Analyze(const std::vector<float>& samples)
    {
        FrequencySpectrum spectrum;
        spectrum.sampleRate = m_sampleRate;

        // Compute magnitude spectrum
        spectrum.magnitudes = FFTProcessor::ComputeMagnitudeSpectrum(samples, m_fftSize);

        // Generate frequency array
        spectrum.frequencies.resize(spectrum.magnitudes.size());
        for (size_t i = 0; i < spectrum.frequencies.size(); ++i)
        {
            spectrum.frequencies[i] = (float)i * m_sampleRate / m_fftSize;
        }

        return spectrum;
    }

    FrequencySignature FrequencyAnalyzer::AnalyzeSignature(const std::vector<float>& buffer, float duration)
    {
        FrequencySignature sig;
        sig.duration = duration;

        // Extract a window from the middle of the buffer
        int startIdx = static_cast<int>((buffer.size() - m_fftSize) / 2);
        if (startIdx < 0) startIdx = 0;

        int endIdx = startIdx + m_fftSize;
        if (endIdx > static_cast<int>(buffer.size()))
        {
            endIdx = static_cast<int>(buffer.size());
        }

        std::vector<float> windowedSamples(buffer.begin() + startIdx, buffer.begin() + endIdx);

        // Get spectrum
        auto spectrum = Analyze(windowedSamples);

        // Find peaks
        auto peaks = FindPeaks(spectrum.magnitudes);

        // Extract top 5 peaks
        size_t numPeaks = peaks.size() < 5 ? peaks.size() : 5;
        for (size_t i = 0; i < numPeaks; ++i)
        {
            sig.peakFrequencies.push_back(peaks[i].frequency);
            sig.peakAmplitudes.push_back(peaks[i].amplitude);
        }

        // Fundamental frequency
        sig.fundamentalFreq = peaks.empty() ? 0 : peaks[0].frequency;

        // Spectral features
        sig.spectralCentroid = CalculateSpectralCentroid(spectrum.magnitudes);
        sig.spectralSpread = CalculateSpectralSpread(spectrum.magnitudes, sig.spectralCentroid);

        return sig;
    }

    std::vector<FrequencyAnalyzer::Peak> FrequencyAnalyzer::FindPeaks(const std::vector<float>& magnitude)
    {
        std::vector<Peak> peaks;

        for (size_t i = 1; i < magnitude.size() - 1; ++i)
        {
            if (magnitude[i] > magnitude[i - 1] && magnitude[i] > magnitude[i + 1] && magnitude[i] > 0.01f)
            {
                Peak p;
                p.frequency = (float)i * m_sampleRate / m_fftSize;
                p.amplitude = magnitude[i];
                peaks.push_back(p);
            }
        }

        // Sort by amplitude
        std::sort(peaks.begin(), peaks.end(), [](const Peak& a, const Peak& b)
            {
                return a.amplitude > b.amplitude;
            });

        return peaks;
    }

    float FrequencyAnalyzer::CalculateSpectralCentroid(const std::vector<float>& magnitude)
    {
        float weightedSum = 0;
        float magnitudeSum = 0;

        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            float freq = (float)i * m_sampleRate / m_fftSize;
            weightedSum += freq * magnitude[i];
            magnitudeSum += magnitude[i];
        }

        return magnitudeSum > 0 ? weightedSum / magnitudeSum : 0;
    }

    float FrequencyAnalyzer::CalculateSpectralSpread(const std::vector<float>& magnitude, float centroid)
    {
        float variance = 0;
        float magnitudeSum = 0;

        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            float freq = (float)i * m_sampleRate / m_fftSize;
            float diff = freq - centroid;
            variance += diff * diff * magnitude[i];
            magnitudeSum += magnitude[i];
        }

        return magnitudeSum > 0 ? std::sqrt(variance / magnitudeSum) : 0;
    }
}