#include "pch.h"
#include "FFTProcessor.h"
#include <cmath>

namespace B3n00n
{
    constexpr float PI = 3.14159265358979323846f;

    std::vector<FFTProcessor::Complex> FFTProcessor::Compute(const std::vector<Complex>& input)
    {
        if (input.size() <= 1) return input;

        int n = NextPowerOfTwo(static_cast<int>(input.size()));
        std::vector<Complex> data(input);
        data.resize(n);

        BitReversalPermutation(data);
        CooleyTukeyFFT(data);

        return data;
    }

    std::vector<float> FFTProcessor::ComputeMagnitudeSpectrum(const std::vector<float>& samples, int fftSize)
    {
        // Apply Hamming window
        std::vector<Complex> fftInput(fftSize);
        int sampleCount = static_cast<int>(samples.size());

        for (int i = 0; i < fftSize && i < sampleCount; i++) 
        {
            float window = 0.54f - 0.46f * cos(2 * PI * i / (fftSize - 1));
            fftInput[i] = Complex(samples[i] * window, 0);
        }

        auto fftOutput = Compute(fftInput);

        // Calculate magnitude spectrum
        std::vector<float> magnitude(fftSize / 2);
        for (int i = 0; i < fftSize / 2; i++)
        {
            magnitude[i] = std::abs(fftOutput[i]) / (fftSize / 2);
        }

        return magnitude;
    }

    int FFTProcessor::NextPowerOfTwo(int n)
    {
        int powerOf2 = 1;
        while (powerOf2 < n) powerOf2 *= 2;
        return powerOf2;
    }

    void FFTProcessor::BitReversalPermutation(std::vector<Complex>& data)
    {
        int n = static_cast<int>(data.size());
        int j = 0;

        for (int i = 1; i < n; i++)
        {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1)
            {
                j ^= bit;
            }
            j ^= bit;

            if (i < j)
            {
                std::swap(data[i], data[j]);
            }
        }
    }

    void FFTProcessor::CooleyTukeyFFT(std::vector<Complex>& data)
    {
        int n = static_cast<int>(data.size());

        for (int len = 2; len <= n; len <<= 1)
        {
            float angle = -2 * PI / len;
            Complex wlen(cos(angle), sin(angle));

            for (int i = 0; i < n; i += len)
            {
                Complex w(1);
                for (int j = 0; j < len / 2; j++)
                {
                    Complex u = data[i + j];
                    Complex v = data[i + j + len / 2] * w;
                    data[i + j] = u + v;
                    data[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }
    }
}