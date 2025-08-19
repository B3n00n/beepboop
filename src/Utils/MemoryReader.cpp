#include "pch.h"
#include "MemoryReader.h"
#include "Logger.h"
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

namespace B3n00n
{
    bool MemoryReader::ReadMemory(uintptr_t address, void* buffer, size_t size)
    {
        if (!address || !buffer || size == 0)
        {
            return false;
        }

        try
        {
            // Check if the memory is readable
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0)
            {
                return false;
            }

            // Check if pages are readable
            if (!(mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY)))
            {
                return false;
            }

            // Direct memory copy since we're in the same process essentially
            memcpy(buffer, reinterpret_cast<const void*>(address), size);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool MemoryReader::ReadInt32(uintptr_t address, int32_t& value)
    {
        return ReadMemory(address, &value, sizeof(int32_t));
    }

    uintptr_t MemoryReader::GetModuleBaseAddress(const std::string& moduleName)
    {
        HMODULE hModule = GetModuleHandleA(moduleName.c_str());
        return reinterpret_cast<uintptr_t>(hModule);
    }

    HANDLE MemoryReader::GetCurrentProcessHandle()
    {
        return GetCurrentProcess();
    }
}