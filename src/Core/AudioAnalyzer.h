#pragma once
#include "IAudioProcessor.h"
#include "../Audio/AudioCapture.h"
#include "../Audio/AudioPlayback.h"
#include "../DSP/FrequencyAnalyzer.h"
#include "../Detection/BeepDetector.h"
#include "../Input/KeyboardMonitor.h"
#include "../Input/IInputHandler.h"
#include "IVehicleDetector.h"
#include <memory>

namespace B3n00n {
    class AudioAnalyzer : public IAudioProcessor, public IInputHandler
    {
    public:
        AudioAnalyzer();
        ~AudioAnalyzer();

        bool Initialize();
        void Start() override;
        void Stop() override;
        bool IsRunning() const override;

        // IAudioProcessor
        void ProcessBuffer(const float* samples, size_t sampleCount) override;

        // IInputHandler
        void OnKeyPress(int virtualKey) override;
        void OnKeyRelease(int virtualKey) override;

    private:
        std::unique_ptr<AudioCapture> m_audioCapture;
        std::unique_ptr<AudioPlayback> m_audioPlayback;
        std::unique_ptr<FrequencyAnalyzer> m_frequencyAnalyzer;
        std::unique_ptr<BeepDetector> m_beepDetector;
        std::unique_ptr<KeyboardMonitor> m_keyboardMonitor;
        std::unique_ptr<IVehicleDetector> m_vehicleDetector;

        // Sound detection state
        std::vector<float> m_soundBuffer;
        std::atomic<bool> m_inSound;
        std::chrono::steady_clock::time_point m_soundStartTime;

        void OnBeepDetected(const FrequencySignature& sig);
        float CalculateEnergy(const float* samples, size_t count) const;
    };
}