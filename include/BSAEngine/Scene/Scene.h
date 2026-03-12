#pragma once

#include <entt/entt.hpp>
#include <string>
#include <memory>
// Gecici Sandbox Shader/VAO injection
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/VertexArray.h"

namespace BSA {

    class Entity;

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        // Hiyerarşi Yardımcıları (Scene Graph)
        BSA::Math::Matrix4 GetWorldTransform(Entity entity);

        // Ana oyun döngüsü metodu (Her frame çalisir)
        void OnUpdate(float ts, std::shared_ptr<BSA::Shader> defaultShader, std::shared_ptr<BSA::VertexArray> defaultVAO);

        // Sandbox/Renderer tarafindan ECS objelerine ulasmak icin (Geçici referans)
        entt::registry& Reg() { return m_Registry; }

    private:
        entt::registry m_Registry;

        friend class Entity;
    };

}
