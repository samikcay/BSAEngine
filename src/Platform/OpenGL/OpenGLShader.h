#pragma once

#include "BSAEngine/Renderer/Shader.h"
#include <cstdint>

namespace BSA {

    // ---------------------------------------------------------------
    // OpenGL Spesifik Shader Uygulaması
    // ---------------------------------------------------------------
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void UploadUniformMat4(const std::string& name, const BSA::Math::Matrix4& matrix) override;

    private:
        uint32_t m_RendererID;
    };

} // namespace BSA
