#include "BSAEngine/Memory/PoolAllocator.h"
#include <cstdlib>
#include <cassert>

namespace BSA {
namespace Memory {

    PoolAllocator::PoolAllocator(std::size_t chunkSize, std::size_t chunkCount)
        : Allocator(chunkSize * chunkCount),
          m_ChunkSize(chunkSize),
          m_ChunkCount(chunkCount),
          m_FreeCount(chunkCount)
    {
        // Chunk boyutu en az bir FreeNode pointer'ı kadar olmalı
        assert(chunkSize >= sizeof(FreeNode) && "PoolAllocator: Chunk boyutu cok kucuk!");

        m_StartPtr = std::malloc(m_TotalSize);
        assert(m_StartPtr && "PoolAllocator: Bellek ayrilamadi!");

        // Serbest listeyi oluştur: her chunk bir sonrakini gösterir
        Reset();
    }

    PoolAllocator::~PoolAllocator() {
        std::free(m_StartPtr);
        m_StartPtr = nullptr;
        m_FreeList = nullptr;
    }

    void* PoolAllocator::Allocate([[maybe_unused]] std::size_t size, [[maybe_unused]] std::size_t alignment) {
        // Pool allocator'da size ve alignment parametreleri kullanılmaz,
        // çünkü tüm bloklar aynı boyuttadır.
        assert(size <= m_ChunkSize && "PoolAllocator: Istenen boyut chunk boyutunu asiyor!");

        if (m_FreeList == nullptr) {
            assert(false && "PoolAllocator: Bos blok kalmadi!");
            return nullptr;
        }

        // Serbest listeden ilk bloğu al
        FreeNode* freeNode = m_FreeList;
        m_FreeList = freeNode->next;

        m_FreeCount--;
        m_UsedSize += m_ChunkSize;
        m_AllocationCount++;

        return reinterpret_cast<void*>(freeNode);
    }

    void PoolAllocator::Deallocate(void* ptr) {
        assert(ptr && "PoolAllocator: nullptr serbest birakma denemesi!");

        // Serbest bırakılan bloğu listenin başına ekle
        FreeNode* freeNode = reinterpret_cast<FreeNode*>(ptr);
        freeNode->next = m_FreeList;
        m_FreeList = freeNode;

        m_FreeCount++;
        m_UsedSize -= m_ChunkSize;
        m_AllocationCount--;
    }

    void PoolAllocator::Reset() {
        // Tüm chunk'ları serbest listeye ekle
        m_FreeList = nullptr;
        for (std::size_t i = 0; i < m_ChunkCount; ++i) {
            std::size_t addr = reinterpret_cast<std::size_t>(m_StartPtr) + (i * m_ChunkSize);
            FreeNode* node = reinterpret_cast<FreeNode*>(addr);
            node->next = m_FreeList;
            m_FreeList = node;
        }

        m_FreeCount = m_ChunkCount;
        m_UsedSize = 0;
        m_AllocationCount = 0;
    }

} // namespace Memory
} // namespace BSA
