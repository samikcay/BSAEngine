#pragma once

#include <string>
#include <cstdint>

namespace BSA {

    // ---------------------------------------------------------------
    // Temel Texture Arayüzü
    // ---------------------------------------------------------------
    class Texture {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
    };

    // ---------------------------------------------------------------
    // 2D Texture Sınıfı (Örn: resimler)
    // ---------------------------------------------------------------
    class Texture2D : public Texture {
    public:
        static Texture2D* Create(const std::string& path);
    };

} // namespace BSA
