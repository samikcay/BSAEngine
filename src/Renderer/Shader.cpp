#include "BSAEngine/Renderer/Shader.h"
#include "../Platform/OpenGL/OpenGLShader.h"
// İleride RendererAPI'ye göre seçim yapılacak

namespace BSA {

    std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
        return std::make_shared<OpenGLShader>(filepath);
    }

    std::shared_ptr<Shader> Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
        return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc);
    }

} // namespace BSA
