#pragma once
#include "../Core/IVehicleDetector.h"
#include <atomic>
#include <thread>
#include <chrono>

namespace B3n00n
{
    class VehicleDetector : public IVehicleDetector
    {
    public:
        VehicleDetector();
        ~VehicleDetector();

        bool Initialize() override;
        void Shutdown() override;
        bool IsInVehicle() const override;

    private:
        struct Config
        {
            std::string moduleName;
            uintptr_t offset;
            int32_t inVehicleValue;
            int32_t outsideVehicleValue;
            int32_t menuVehicleValue;
            std::chrono::milliseconds pollInterval;
        };

        Config m_config;
        uintptr_t m_targetAddress;
        mutable std::atomic<int32_t> m_currentValue;
        std::atomic<bool> m_isRunning;
        std::thread m_pollThread;

        void PollVehicleState();
        bool ValidateAddress();
    };
}