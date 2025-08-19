#include "pch.h"
#include "VehicleDetector.h"
#include "../Utils/MemoryReader.h"
#include "../Utils/Logger.h"

namespace B3n00n
{
    VehicleDetector::VehicleDetector() : m_targetAddress(0), m_currentValue(0), m_isRunning(false)
    {
        m_config.moduleName = "CaravanSandWitch-Win64-Shipping.exe";
        m_config.offset = 0x7DCF8B8;
        m_config.inVehicleValue = 145;
        m_config.outsideVehicleValue = 146;
        m_config.menuVehicleValue = 1;
        m_config.pollInterval = std::chrono::milliseconds(100);
    }

    VehicleDetector::~VehicleDetector()
    {
        Shutdown();
    }

    bool VehicleDetector::Initialize()
    {
        uintptr_t baseAddress = MemoryReader::GetModuleBaseAddress(m_config.moduleName);
        if (!baseAddress)
        {
            return false;
        }

        m_targetAddress = baseAddress + m_config.offset;

        if (!ValidateAddress())
        {
            return false;
        }

        m_isRunning = true;
        m_pollThread = std::thread(&VehicleDetector::PollVehicleState, this);

        return true;
    }

    void VehicleDetector::Shutdown()
    {
        m_isRunning = false;
        if (m_pollThread.joinable())
        {
            m_pollThread.join();
        }
    }

    bool VehicleDetector::IsInVehicle() const
    {
        return m_currentValue.load() == m_config.inVehicleValue;
    }

    void VehicleDetector::PollVehicleState()
    {
        while (m_isRunning)
        {
            int32_t value = 0;
            if (MemoryReader::ReadInt32(m_targetAddress, value))
            {
                m_currentValue = value;
            }

            std::this_thread::sleep_for(m_config.pollInterval);
        }
    }

    bool VehicleDetector::ValidateAddress()
    {
        int32_t value = 0;

        if (!MemoryReader::ReadInt32(m_targetAddress, value))
        {
            return false;
        }

        m_currentValue = value;

        return (value == m_config.inVehicleValue || value == m_config.outsideVehicleValue || value == m_config.menuVehicleValue);
    }
}