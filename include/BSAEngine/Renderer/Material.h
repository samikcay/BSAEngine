#pragma once

#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <memory>

namespace BSA {

    class Material {
    public:
        Material(const std::shared_ptr<Shader>& shader, const std::string& name = "Untitled Material");
        ~Material() = default;

        void Bind();

        void Set(const std::string& name, float value);
        void Set(const std::string& name, int value);
        void Set(const std::string& name, const glm::vec2& value);
        void Set(const std::string& name, const glm::vec3& value);
        void Set(const std::string& name, const glm::vec4& value);
        void Set(const std::string& name, const glm::mat4& value);
        void Set(const std::string& name, const std::shared_ptr<Texture>& texture, uint32_t slot);

        const std::string& GetName() const { return m_Name; }
        std::shared_ptr<Shader> GetShader() const { return m_Shader; }

        std::map<std::string, float>& GetFloats() { return m_Floats; }
        std::map<std::string, int>& GetInts() { return m_Ints; }
        std::map<std::string, glm::vec2>& GetVec2s() { return m_Vec2s; }
        std::map<std::string, glm::vec3>& GetVec3s() { return m_Vec3s; }
        std::map<std::string, glm::vec4>& GetVec4s() { return m_Vec4s; }
        std::map<std::string, glm::mat4>& GetMat4s() { return m_Mat4s; }
        
        struct TextureSlot {
            std::shared_ptr<Texture> Tex;
            uint32_t Slot;
        };
        std::map<std::string, TextureSlot>& GetTextures() { return m_Textures; }

    private:
        std::string m_Name;
        std::shared_ptr<Shader> m_Shader;

        std::map<std::string, float> m_Floats;
        std::map<std::string, int> m_Ints;
        std::map<std::string, glm::vec2> m_Vec2s;
        std::map<std::string, glm::vec3> m_Vec3s;
        std::map<std::string, glm::vec4> m_Vec4s;
        std::map<std::string, glm::mat4> m_Mat4s;
        std::map<std::string, TextureSlot> m_Textures;
    };

} // namespace BSA
