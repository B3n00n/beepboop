#pragma once

namespace B3n00n
{
    namespace Constants
    {
        constexpr int FFT_SIZE = 2048;
        constexpr float SAMPLE_RATE = 48000.0f;

        constexpr float SOUND_THRESHOLD = 0.0005f;
        constexpr float SILENCE_THRESHOLD = 0.0002f;

        constexpr int MIN_BEEP_DURATION_MS = 90;
        constexpr int MAX_BEEP_DURATION_MS = 140;

        constexpr float BEEP_FREQ1_MIN = 750.0f;
        constexpr float BEEP_FREQ1_MAX = 900.0f;

        constexpr float BEEP_FREQ2_MIN = 850.0f;
        constexpr float BEEP_FREQ2_MAX = 980.0f;

        constexpr float CENTROID_MIN = 3500.0f;
        constexpr float CENTROID_MAX = 5000.0f;
        constexpr float SPREAD_MIN = 5500.0f;
        constexpr float SPREAD_MAX = 6800.0f;

        constexpr float MIN_PEAK_AMPLITUDE = 0.05f;

        constexpr size_t DEFAULT_THREAD_COUNT = 2;

        constexpr int VK_SPACE_KEY = VK_SPACE;
    }
}