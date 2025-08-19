#include "pch.h"
#include "AudioCapture.h"
#include "../Utils/Logger.h"
#include "../Utils/Constants.h"

namespace B3n00n
{
    AudioCapture::AudioCapture() : m_audioClient(nullptr), m_captureClient(nullptr), m_waveFormat(nullptr), m_isCapturing(false) { }

    AudioCapture::~AudioCapture()
    {
        Stop();
        Cleanup();
    }

    bool AudioCapture::Initialize()
    {
        HRESULT hr = CoInitialize(nullptr);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return false;

        IMMDeviceEnumerator* enumerator = nullptr;
        IMMDevice* device = nullptr;

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
        if (FAILED(hr)) return false;

        hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        if (FAILED(hr))
        {
            enumerator->Release();
            return false;
        }

        hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_audioClient);

        if (FAILED(hr))
        {
            device->Release();
            enumerator->Release();
            return false;
        }

        hr = m_audioClient->GetMixFormat(&m_waveFormat);
        if (FAILED(hr))
        {
            Cleanup();
            device->Release();
            enumerator->Release();
            return false;
        }

        hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, m_waveFormat, nullptr);

        if (FAILED(hr))
        {
            Cleanup();
            device->Release();
            enumerator->Release();
            return false;
        }

        hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_captureClient);

        if (FAILED(hr))
        {
            Cleanup();
            device->Release();
            enumerator->Release();
            return false;
        }

        device->Release();
        enumerator->Release();
        return true;
    }

    void AudioCapture::SetBufferCallback(BufferCallback callback)
    {
        m_callback = callback;
    }

    void AudioCapture::Start()
    {
        if (!m_audioClient) return;

        HRESULT hr = m_audioClient->Start();
        if (SUCCEEDED(hr))
        {
            m_isCapturing = true;
            m_captureThread = std::thread(&AudioCapture::CaptureLoop, this);
        }
    }

    void AudioCapture::Stop()
    {
        m_isCapturing = false;

        if (m_captureThread.joinable())
        {
            m_captureThread.join();
        }

        if (m_audioClient)
        {
            m_audioClient->Stop();
        }
    }

    bool AudioCapture::IsCapturing() const
    {
        return m_isCapturing;
    }

    void AudioCapture::CaptureLoop()
    {
        while (m_isCapturing)
        {
            UINT32 packetLength = 0;
            HRESULT hr = m_captureClient->GetNextPacketSize(&packetLength);

            if (SUCCEEDED(hr) && packetLength > 0)
            {
                BYTE* pData;
                UINT32 numFramesAvailable;
                DWORD flags;

                hr = m_captureClient->GetBuffer(&pData, &numFramesAvailable,
                    &flags, nullptr, nullptr);
                if (SUCCEEDED(hr)) {
                    if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && m_callback)
                    {
                        float* samples = reinterpret_cast<float*>(pData);
                        m_callback(samples, numFramesAvailable);
                    }

                    m_captureClient->ReleaseBuffer(numFramesAvailable);
                }
            }

            Sleep(10);
        }
    }

    void AudioCapture::Cleanup()
    {
        if (m_captureClient)
        {
            m_captureClient->Release();
            m_captureClient = nullptr;
        }

        if (m_audioClient)
        {
            m_audioClient->Release();
            m_audioClient = nullptr;
        }

        if (m_waveFormat)
        {
            CoTaskMemFree(m_waveFormat);
            m_waveFormat = nullptr;
        }
    }
}