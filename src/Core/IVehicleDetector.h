#pragma once

namespace B3n00n {
    class IVehicleDetector
    {
    public:
        virtual ~IVehicleDetector() = default;
        virtual bool IsInVehicle() const = 0;
        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;
    };
}