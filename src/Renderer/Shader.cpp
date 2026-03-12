#include "BSAEngine/Renderer/Shader.h"
#include "../Platform/OpenGL/OpenGLShader.h"
// İleride RendererAPI'ye göre seçim yapılacak

namespace BSA {

    Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
        return new OpenGLShader(vertexSrc, fragmentSrc);
    }

} // namespace BSA
