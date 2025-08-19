#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <memory>

namespace B3n00n
{
    class AudioPlayback
    {
    public:
        AudioPlayback();
        ~AudioPlayback();

        bool LoadWaveFile(const std::string& filepath);
        bool LoadWaveFromResource(int resourceId);
        void Play();
        void Stop();
        bool IsPlaying() const;

    private:
        struct WaveData
        {
            std::vector<BYTE> data;
            WAVEFORMATEX format;
        };

        std::unique_ptr<WaveData> m_waveData;
        HWAVEOUT m_waveOut;
        WAVEHDR m_waveHeader;
        std::atomic<bool> m_isPlaying;

        bool ParseWaveFile(const std::string& filepath);
        bool ParseWaveFromMemory(BYTE* data, DWORD size);
    };
}