#pragma once

#include "BSAEngine/Renderer/Shader.h"
#include <cstdint>

namespace BSA {

    // ---------------------------------------------------------------
    // OpenGL Spesifik Shader Uygulaması
    // ---------------------------------------------------------------
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void UploadUniformInt(const std::string& name, int value) override;
        virtual void UploadUniformIntArray(const std::string& name, int* values, uint32_t count) override;

        virtual void UploadUniformFloat(const std::string& name, float value) override;
        virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) override;

        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
        virtual void UploadUniformMat4(const std::string& name, const BSA::Math::Matrix4& matrix) override;

        virtual AssetType GetType() const override { return AssetType::Shader; }

    private:
        virtual int GetUniformLocation(const std::string& name) const override;

    private:
        uint32_t m_RendererID;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };

} // namespace BSA
