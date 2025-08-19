#include "pch.h"
#include "AudioAnalyzer.h"
#include "../Utils/Logger.h"
#include "../Utils/Constants.h"
#include "../Detection/VehicleDetector.h"
#include "../../resource.h"

namespace B3n00n {
    AudioAnalyzer::AudioAnalyzer() : m_inSound(false)
    {
        m_audioCapture = std::make_unique<AudioCapture>();
        m_audioPlayback = std::make_unique<AudioPlayback>();
        m_frequencyAnalyzer = std::make_unique<FrequencyAnalyzer>(Constants::SAMPLE_RATE, Constants::FFT_SIZE);
        m_beepDetector = std::make_unique<BeepDetector>();
        m_keyboardMonitor = std::make_unique<KeyboardMonitor>();
        m_vehicleDetector = std::make_unique<VehicleDetector>();
    }

    AudioAnalyzer::~AudioAnalyzer()
    {
        Stop();
    }

    bool AudioAnalyzer::Initialize()
    {
        if (!m_audioCapture->Initialize())
        {
            LOG_ERROR("Failed to initialize audio capture");
            return false;
        }

        m_audioCapture->SetBufferCallback([this](const float* samples, size_t count) { ProcessBuffer(samples, count); });

        m_beepDetector->SetDetectionCallback([this](const FrequencySignature& sig) { OnBeepDetected(sig); });

        if (!m_audioPlayback->LoadWaveFromResource(IDR_BEEP_WAVE))
        {
            // Silent fail if wave resource not found
        }

        if (!m_vehicleDetector->Initialize())
        {
            LOG_WARNING("Vehicle detector initialization failed - will detect all beeps");
        }

        m_keyboardMonitor->SetInputHandler(std::shared_ptr<IInputHandler>(this, [](IInputHandler*) {}));

        return true;
    }

    void AudioAnalyzer::Start()
    {
        m_audioCapture->Start();
        m_keyboardMonitor->Start();
    }

    void AudioAnalyzer::Stop()
    {
        m_keyboardMonitor->Stop();
        m_audioCapture->Stop();
        if (m_vehicleDetector)
        {
            m_vehicleDetector->Shutdown();
        }
    }

    bool AudioAnalyzer::IsRunning() const
    {
        return m_audioCapture->IsCapturing();
    }

    void AudioAnalyzer::ProcessBuffer(const float* samples, size_t sampleCount)
    {
        float energy = CalculateEnergy(samples, sampleCount);

        if (energy > Constants::SOUND_THRESHOLD)
        {
            if (!m_inSound)
            {
                m_inSound = true;
                m_soundStartTime = std::chrono::steady_clock::now();
                m_soundBuffer.clear();
            }

            m_soundBuffer.insert(m_soundBuffer.end(), samples, samples + sampleCount);
        }
        else if (m_inSound && energy < Constants::SILENCE_THRESHOLD)
        {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_soundStartTime).count();

            if (duration > 50 && m_soundBuffer.size() > Constants::FFT_SIZE)
            {
                auto signature = m_frequencyAnalyzer->AnalyzeSignature(m_soundBuffer, static_cast<float>(duration));

                // Check for our beep boop beep
                m_beepDetector->IsBeepSound(signature);
            }

            m_inSound = false;
        }
    }

    void AudioAnalyzer::OnKeyPress(int virtualKey)
    {
        if (virtualKey == Constants::VK_SPACE_KEY)
        {
            m_audioPlayback->Play();
        }
    }

    void AudioAnalyzer::OnKeyRelease(int virtualKey)
    {
        // Unused
    }

    void AudioAnalyzer::OnBeepDetected(const FrequencySignature& sig)
    {
        bool inVehicle = m_vehicleDetector && m_vehicleDetector->IsInVehicle();
        Logger::Instance().LogBeepDetection(sig, inVehicle);
    }

    float AudioAnalyzer::CalculateEnergy(const float* samples, size_t count) const
    {
        float sum = 0;
        for (size_t i = 0; i < count; i++)
        {
            sum += samples[i] * samples[i];
        }
        return std::sqrt(sum / count);
    }
}