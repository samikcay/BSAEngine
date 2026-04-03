#include "BSAEngine/Renderer/Texture.h"
#include "../Platform/OpenGL/OpenGLTexture.h"

namespace BSA {

    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) {
        return std::make_shared<OpenGLTexture2D>(width, height);
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& filepath) {
        return std::make_shared<OpenGLTexture2D>(filepath);
    }

} // namespace BSA
