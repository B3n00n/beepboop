#pragma once
#include <windows.h>
#include <string>
#include <cstdint>

namespace B3n00n
{
    class MemoryReader
    {
    public:
        static bool ReadMemory(uintptr_t address, void* buffer, size_t size);
        static bool ReadInt32(uintptr_t address, int32_t& value);
        static uintptr_t GetModuleBaseAddress(const std::string& moduleName);

    private:
        static HANDLE GetCurrentProcessHandle();
    };
}