#pragma once

#include "BSAEngine/Asset/Asset.h"
#include "BSAEngine/Math/Matrix4.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace BSA {

    // ---------------------------------------------------------------
    // Shader (Gölgelendirici) Soyut Arayüzü
    // ---------------------------------------------------------------
    class Shader : public Asset {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void UploadUniformInt(const std::string& name, int value) = 0;
        virtual void UploadUniformIntArray(const std::string& name, int* values, uint32_t count) = 0;

        virtual void UploadUniformFloat(const std::string& name, float value) = 0;
        virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;
        virtual void UploadUniformMat4(const std::string& name, const BSA::Math::Matrix4& matrix) = 0;

        virtual AssetType GetType() const override { return AssetType::Shader; }

        // Factory metotları
        static std::shared_ptr<Shader> Create(const std::string& filepath);
        static std::shared_ptr<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);

    protected:
        virtual int GetUniformLocation(const std::string& name) const = 0;
    };

} // namespace BSA
