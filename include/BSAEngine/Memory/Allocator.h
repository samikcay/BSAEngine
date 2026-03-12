#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <new>

namespace BSA {
namespace Memory {

    // ---------------------------------------------------------------
    // Temel Allocator arayüzü
    // ---------------------------------------------------------------
    class Allocator {
    public:
        Allocator(std::size_t totalSize)
            : m_TotalSize(totalSize), m_UsedSize(0), m_AllocationCount(0) {}

        virtual ~Allocator() = default;

        virtual void* Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) = 0;
        virtual void  Deallocate(void* ptr) = 0;
        virtual void  Reset() = 0;

        std::size_t GetTotalSize()       const { return m_TotalSize; }
        std::size_t GetUsedSize()        const { return m_UsedSize; }
        std::size_t GetAllocationCount() const { return m_AllocationCount; }

        // Kopyalama ve taşıma yasak
        Allocator(const Allocator&) = delete;
        Allocator& operator=(const Allocator&) = delete;

    protected:
        std::size_t m_TotalSize;
        std::size_t m_UsedSize;
        std::size_t m_AllocationCount;

        // Verilen adresi hizalama (alignment) sınırına yuvarlar
        static std::size_t AlignForward(std::size_t address, std::size_t alignment) {
            std::size_t remainder = address % alignment;
            if (remainder == 0) return address;
            return address + (alignment - remainder);
        }

        static std::size_t AlignForwardPadding(std::size_t address, std::size_t alignment) {
            return AlignForward(address, alignment) - address;
        }
    };

} // namespace Memory
} // namespace BSA
