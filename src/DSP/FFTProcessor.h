#pragma once
#include <vector>
#include <complex>

namespace B3n00n
{
    class FFTProcessor
    {
    public:
        using Complex = std::complex<float>;

        static std::vector<Complex> Compute(const std::vector<Complex>& input);
        static std::vector<float> ComputeMagnitudeSpectrum(const std::vector<float>& samples,
            int fftSize);

    private:
        static int NextPowerOfTwo(int n);
        static void BitReversalPermutation(std::vector<Complex>& data);
        static void CooleyTukeyFFT(std::vector<Complex>& data);
    };
}