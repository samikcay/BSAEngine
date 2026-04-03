#include "BSAEngine/Renderer/Material.h"

namespace BSA {

    Material::Material(const std::shared_ptr<Shader>& shader, const std::string& name)
        : m_Shader(shader), m_Name(name)
    {
    }

    void Material::Bind() {
        m_Shader->Bind();

        for (auto& [name, value] : m_Floats) m_Shader->UploadUniformFloat(name, value);
        for (auto& [name, value] : m_Ints) m_Shader->UploadUniformInt(name, value);
        for (auto& [name, value] : m_Vec2s) m_Shader->UploadUniformFloat2(name, value);
        for (auto& [name, value] : m_Vec3s) m_Shader->UploadUniformFloat3(name, value);
        for (auto& [name, value] : m_Vec4s) m_Shader->UploadUniformFloat4(name, value);
        for (auto& [name, value] : m_Mat4s) m_Shader->UploadUniformMat4(name, value);

        for (auto& [name, slot] : m_Textures) {
            slot.Tex->Bind(slot.Slot);
            m_Shader->UploadUniformInt(name, slot.Slot);
        }
    }

    void Material::Set(const std::string& name, float value) { m_Floats[name] = value; }
    void Material::Set(const std::string& name, int value) { m_Ints[name] = value; }
    void Material::Set(const std::string& name, const glm::vec2& value) { m_Vec2s[name] = value; }
    void Material::Set(const std::string& name, const glm::vec3& value) { m_Vec3s[name] = value; }
    void Material::Set(const std::string& name, const glm::vec4& value) { m_Vec4s[name] = value; }
    void Material::Set(const std::string& name, const glm::mat4& value) { m_Mat4s[name] = value; }
    void Material::Set(const std::string& name, const std::shared_ptr<Texture>& texture, uint32_t slot) {
        m_Textures[name] = { texture, slot };
    }

} // namespace BSA
