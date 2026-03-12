#include "BSAEngine/Memory/LinearAllocator.h"
#include <cstdlib>
#include <cassert>

namespace BSA {
namespace Memory {

    LinearAllocator::LinearAllocator(std::size_t totalSize)
        : Allocator(totalSize)
    {
        m_StartPtr = std::malloc(totalSize);
        assert(m_StartPtr && "LinearAllocator: Bellek ayrilamadi!");
        m_Offset = 0;
    }

    LinearAllocator::~LinearAllocator() {
        std::free(m_StartPtr);
        m_StartPtr = nullptr;
    }

    void* LinearAllocator::Allocate(std::size_t size, std::size_t alignment) {
        // Mevcut pozisyonun adresini hesapla
        std::size_t currentAddr = reinterpret_cast<std::size_t>(m_StartPtr) + m_Offset;
        std::size_t padding = AlignForwardPadding(currentAddr, alignment);

        // Yeterli alan var mı kontrol et
        if (m_Offset + padding + size > m_TotalSize) {
            assert(false && "LinearAllocator: Yetersiz bellek!");
            return nullptr;
        }

        m_Offset += padding;
        std::size_t alignedAddr = reinterpret_cast<std::size_t>(m_StartPtr) + m_Offset;
        m_Offset += size;

        m_UsedSize = m_Offset;
        m_AllocationCount++;

        return reinterpret_cast<void*>(alignedAddr);
    }

    void LinearAllocator::Deallocate([[maybe_unused]] void* ptr) {
        // Linear allocator tek tek serbest bırakmayı desteklemez.
        // Tüm bellek Reset() ile serbest bırakılır.
    }

    void LinearAllocator::Reset() {
        m_Offset = 0;
        m_UsedSize = 0;
        m_AllocationCount = 0;
    }

} // namespace Memory
} // namespace BSA
