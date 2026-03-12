#pragma once

#include "Allocator.h"

namespace BSA {
namespace Memory {

    // ---------------------------------------------------------------
    // Linear Allocator
    // Belleği doğrusal olarak ayırır, tek tek serbest bırakılamaz.
    // Tüm bellek sadece Reset() ile serbest bırakılır.
    // En hızlı allocator tipi.
    // ---------------------------------------------------------------
    class LinearAllocator : public Allocator {
    public:
        LinearAllocator(std::size_t totalSize);
        ~LinearAllocator() override;

        void* Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
        void  Deallocate(void* ptr) override;
        void  Reset() override;

    private:
        void*       m_StartPtr = nullptr;
        std::size_t m_Offset   = 0;
    };

} // namespace Memory
} // namespace BSA
