#pragma once

#include "Allocator.h"

namespace BSA {
namespace Memory {

    // ---------------------------------------------------------------
    // Stack Allocator
    // LIFO (Son Giren İlk Çıkar) düzeninde bellek ayırma/serbest bırakma.
    // Her ayırmanın başına bir header eklenir, geri dönüş için kullanılır.
    // ---------------------------------------------------------------
    class StackAllocator : public Allocator {
    public:
        StackAllocator(std::size_t totalSize);
        ~StackAllocator() override;

        void* Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
        void  Deallocate(void* ptr) override;
        void  Reset() override;

    private:
        // Her ayırmanın önüne eklenen header
        struct AllocationHeader {
            std::size_t padding; // Hizalama için eklenen dolgu miktarı
        };

        void*       m_StartPtr = nullptr;
        std::size_t m_Offset   = 0;
    };

} // namespace Memory
} // namespace BSA
