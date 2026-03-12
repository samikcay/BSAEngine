#include "BSAEngine/Renderer/Framebuffer.h"
#include "BSAEngine/Log/Log.h"
// TODO: İleride Renderer::GetAPI() tabanlı bir yapı kuracağız. Şimdilik doğrudan OpenGL dahil ediliyor.
#include "../Platform/OpenGL/OpenGLFramebuffer.h"

namespace BSA {

    Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec) {
        // Şimdilik sadece OpenGL desteklediğimiz varsayımıyla direkt dönüyoruz.
        return new OpenGLFramebuffer(spec);
    }

}
