#include "pch.h"
#include "AudioPlayback.h"
#include "../Utils/Logger.h"
#include <fstream>
#include <algorithm>

namespace B3n00n
{
    AudioPlayback::AudioPlayback() : m_waveOut(nullptr), m_isPlaying(false)
    {
        ZeroMemory(&m_waveHeader, sizeof(WAVEHDR));
    }

    AudioPlayback::~AudioPlayback()
    {
        Stop();

        if (m_waveOut)
        {
            waveOutClose(m_waveOut);
        }
    }

    bool AudioPlayback::LoadWaveFile(const std::string& filepath)
    {
        return ParseWaveFile(filepath);
    }

    bool AudioPlayback::LoadWaveFromResource(int resourceId)
    {
        HMODULE hModule = GetModuleHandle(L"beep.dll");

        if (!hModule)
        {
            return false;
        }

        HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(resourceId), L"WAVE");

        if (!hResource)
        {
            return false;
        }

        HGLOBAL hLoadedResource = LoadResource(hModule, hResource);

        if (!hLoadedResource)
        {
            return false;
        }

        LPVOID pResourceData = LockResource(hLoadedResource);
        if (!pResourceData)
        {
            return false;
        }

        DWORD resourceSize = SizeofResource(hModule, hResource);

        return ParseWaveFromMemory((BYTE*)pResourceData, resourceSize);
    }

    bool AudioPlayback::ParseWaveFromMemory(BYTE* data, DWORD size)
    {
        if (size < 44)
        {
            return false;
        }

        // Check RIFF header
        if (memcmp(data, "RIFF", 4) != 0) 
        {
            return false;
        }

        // Check WAVE format
        if (memcmp(data + 8, "WAVE", 4) != 0)
        {
            return false;
        }

        m_waveData = std::make_unique<WaveData>();

        DWORD offset = 12;
        while (offset < size - 8)
        {
            char chunkId[4];
            memcpy(chunkId, data + offset, 4);
            offset += 4;

            DWORD chunkSize;
            memcpy(&chunkSize, data + offset, 4);
            offset += 4;

            if (memcmp(chunkId, "fmt ", 4) == 0)
            {
                // Read format chunk
                DWORD bytesToRead = (std::min)(chunkSize, static_cast<DWORD>(sizeof(WAVEFORMATEX)));
                memcpy(&m_waveData->format, data + offset, bytesToRead);
            }
            else if (memcmp(chunkId, "data", 4) == 0)
            {
                // Read data chunk
                m_waveData->data.resize(chunkSize);
                memcpy(m_waveData->data.data(), data + offset, chunkSize);
            }

            offset += chunkSize;
        }

        return true;
    }

    void AudioPlayback::Play()
    {
        if (!m_waveData || m_isPlaying) return;

        MMRESULT result = waveOutOpen(&m_waveOut, WAVE_MAPPER, &m_waveData->format, 0, 0, CALLBACK_NULL);

        if (result != MMSYSERR_NOERROR)
        {
            return;
        }

        m_waveHeader.lpData = reinterpret_cast<LPSTR>(m_waveData->data.data());
        m_waveHeader.dwBufferLength = static_cast<DWORD>(m_waveData->data.size());
        m_waveHeader.dwFlags = 0;

        result = waveOutPrepareHeader(m_waveOut, &m_waveHeader, sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR)
        {
            waveOutClose(m_waveOut);
            m_waveOut = nullptr;
            return;
        }

        result = waveOutWrite(m_waveOut, &m_waveHeader, sizeof(WAVEHDR));
        if (result != MMSYSERR_NOERROR)
        {
            waveOutUnprepareHeader(m_waveOut, &m_waveHeader, sizeof(WAVEHDR));
            waveOutClose(m_waveOut);
            m_waveOut = nullptr;
            return;
        }

        m_isPlaying = true;

        // Start a thread to monitor playback completion
        std::thread([this]()
            {
            while (m_isPlaying && !(m_waveHeader.dwFlags & WHDR_DONE))
            {
                Sleep(10);
            }

            if (m_waveOut)
            {
                waveOutUnprepareHeader(m_waveOut, &m_waveHeader, sizeof(WAVEHDR));
                waveOutClose(m_waveOut);
                m_waveOut = nullptr;
            }

            m_isPlaying = false;
            }).detach();
    }

    void AudioPlayback::Stop()
    {
        if (m_waveOut && m_isPlaying)
        {
            waveOutReset(m_waveOut);
            m_isPlaying = false;
        }
    }

    bool AudioPlayback::IsPlaying() const
    {
        return m_isPlaying;
    }

    bool AudioPlayback::ParseWaveFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            return false;
        }

        // Read RIFF header
        char chunkId[4];
        file.read(chunkId, 4);
        if (std::string(chunkId, 4) != "RIFF")
        {
            return false;
        }

        uint32_t chunkSize;
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        char format[4];
        file.read(format, 4);
        if (std::string(format, 4) != "WAVE")
        {
            return false;
        }

        m_waveData = std::make_unique<WaveData>();

        // Read chunks
        while (!file.eof()) 
        {
            char subChunkId[4];
            file.read(subChunkId, 4);
            if (file.eof()) break;

            uint32_t subChunkSize;
            file.read(reinterpret_cast<char*>(&subChunkSize), 4);

            if (std::string(subChunkId, 4) == "fmt ")
            {
                // Read format chunk
                uint32_t bytesToRead = (std::min)(subChunkSize, static_cast<uint32_t>(sizeof(WAVEFORMATEX)));
                file.read(reinterpret_cast<char*>(&m_waveData->format), bytesToRead);

                if (subChunkSize > sizeof(WAVEFORMATEX))
                {
                    file.seekg(subChunkSize - sizeof(WAVEFORMATEX), std::ios::cur);
                }
            }
            else if (std::string(subChunkId, 4) == "data")
            {
                // Read data chunk
                m_waveData->data.resize(subChunkSize);
                file.read(reinterpret_cast<char*>(m_waveData->data.data()), subChunkSize);
            }
            else
            {
                // Skip unknown chunk
                file.seekg(subChunkSize, std::ios::cur);
            }
        }

        return true;
    }
}