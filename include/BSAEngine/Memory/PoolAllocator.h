#pragma once

#include "Allocator.h"

namespace BSA {
namespace Memory {

    // ---------------------------------------------------------------
    // Pool Allocator
    // Sabit boyutlu bloklar halinde bellek ayırır.
    // Serbest bloklar bağlı liste (free list) ile izlenir.
    // Aynı boyuttaki nesneler için idealdir (ECS bileşenleri vb.)
    // ---------------------------------------------------------------
    class PoolAllocator : public Allocator {
    public:
        PoolAllocator(std::size_t chunkSize, std::size_t chunkCount);
        ~PoolAllocator() override;

        void* Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
        void  Deallocate(void* ptr) override;
        void  Reset() override;

        std::size_t GetChunkSize()  const { return m_ChunkSize; }
        std::size_t GetChunkCount() const { return m_ChunkCount; }
        std::size_t GetFreeCount()  const { return m_FreeCount; }

    private:
        // Serbest bloklar arası bağlı liste düğümü
        struct FreeNode {
            FreeNode* next;
        };

        void*       m_StartPtr   = nullptr;
        FreeNode*   m_FreeList   = nullptr;
        std::size_t m_ChunkSize  = 0;
        std::size_t m_ChunkCount = 0;
        std::size_t m_FreeCount  = 0;
    };

} // namespace Memory
} // namespace BSA
