#include "BSAEngine/Memory/StackAllocator.h"
#include <cstdlib>
#include <cassert>

namespace BSA {
namespace Memory {

    StackAllocator::StackAllocator(std::size_t totalSize)
        : Allocator(totalSize)
    {
        m_StartPtr = std::malloc(totalSize);
        assert(m_StartPtr && "StackAllocator: Bellek ayrilamadi!");
        m_Offset = 0;
    }

    StackAllocator::~StackAllocator() {
        std::free(m_StartPtr);
        m_StartPtr = nullptr;
    }

    void* StackAllocator::Allocate(std::size_t size, std::size_t alignment) {
        // Header + hizalama için gereken alanı hesapla
        std::size_t currentAddr = reinterpret_cast<std::size_t>(m_StartPtr) + m_Offset;

        // Header'ı yerleştirdikten sonra hizalama dolgusu hesapla
        std::size_t headerSize = sizeof(AllocationHeader);
        std::size_t padding = AlignForwardPadding(currentAddr + headerSize, alignment);
        std::size_t totalPadding = headerSize + padding;

        // Yeterli alan var mı kontrol et
        if (m_Offset + totalPadding + size > m_TotalSize) {
            assert(false && "StackAllocator: Yetersiz bellek!");
            return nullptr;
        }

        // Header'ı yaz
        std::size_t headerAddr = currentAddr + padding;
        AllocationHeader* header = reinterpret_cast<AllocationHeader*>(headerAddr);
        header->padding = totalPadding;

        // Hizalanmış adresi hesapla
        std::size_t alignedAddr = currentAddr + totalPadding;
        m_Offset += totalPadding + size;

        m_UsedSize = m_Offset;
        m_AllocationCount++;

        return reinterpret_cast<void*>(alignedAddr);
    }

    void StackAllocator::Deallocate(void* ptr) {
        assert(ptr && "StackAllocator: nullptr serbest birakma denemesi!");

        // Header'ı geri oku
        std::size_t ptrAddr = reinterpret_cast<std::size_t>(ptr);
        std::size_t headerAddr = ptrAddr - sizeof(AllocationHeader);
        AllocationHeader* header = reinterpret_cast<AllocationHeader*>(headerAddr);

        // Offset'i geri al
        std::size_t startAddr = reinterpret_cast<std::size_t>(m_StartPtr);
        m_Offset = (ptrAddr - header->padding) - startAddr;

        m_UsedSize = m_Offset;
        m_AllocationCount--;
    }

    void StackAllocator::Reset() {
        m_Offset = 0;
        m_UsedSize = 0;
        m_AllocationCount = 0;
    }

} // namespace Memory
} // namespace BSA
