#include "BSAEngine/Renderer/Texture.h"
#include "../Platform/OpenGL/OpenGLTexture.h"

namespace BSA {

    Texture2D* Texture2D::Create(const std::string& path) {
        return new OpenGLTexture2D(path);
    }

} // namespace BSA
