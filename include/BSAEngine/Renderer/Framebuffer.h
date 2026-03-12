#pragma once

#include <cstdint>
#include <memory>

namespace BSA {

    struct FramebufferSpecification {
        uint32_t Width, Height;
        // İleride Multi-sampling/MSAA parametreleri buraya gelebilir
        uint32_t Samples = 1;
        
        bool SwapChainTarget = false; // Ana pencereye mi çiziyoruz?
    };

    class Framebuffer {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetColorAttachmentRendererID() const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;

        static Framebuffer* Create(const FramebufferSpecification& spec);
    };

}
