#pragma once
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <memory>
#include <functional>

namespace B3n00n
{
    class AudioCapture
    {
    public:
        using BufferCallback = std::function<void(const float*, size_t)>;

        AudioCapture();
        ~AudioCapture();

        bool Initialize();
        void SetBufferCallback(BufferCallback callback);
        void Start();
        void Stop();
        bool IsCapturing() const;

    private:
        IAudioClient* m_audioClient;
        IAudioCaptureClient* m_captureClient;
        WAVEFORMATEX* m_waveFormat;
        std::atomic<bool> m_isCapturing;
        BufferCallback m_callback;
        std::thread m_captureThread;

        void CaptureLoop();
        void Cleanup();
    };
}