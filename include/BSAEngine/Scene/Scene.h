#pragma once

#include <entt/entt.hpp>
#include <string>
#include <memory>
// Gecici Sandbox Shader/VAO injection
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/VertexArray.h"

class b2World; // Box2D forward declaration

namespace BSA {

    class Entity;
    class PhysicsContactListener;

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        // Hiyerarşi Yardımcıları (Scene Graph)
        BSA::Math::Matrix4 GetWorldTransform(Entity entity);

        // Fizik ve sabit zaman odaklı mantık güncellemeleri
        void OnFixedUpdate(float fixedTimeStep);

        // Ana oyun döngüsü metodu (Her frame çalisir)
        void OnUpdate(float ts, const glm::mat4& viewProjection, const glm::vec3& cameraPosition, std::shared_ptr<Shader> defaultShader, std::shared_ptr<VertexArray> defaultVAO);
        void OnUpdate(float ts, std::shared_ptr<Shader> defaultShader, std::shared_ptr<VertexArray> defaultVAO); // Overload for primary camera

        void OnViewportResize(uint32_t width, uint32_t height);

        // Sandbox/Renderer tarafindan ECS objelerine ulasmak icin (Geçici referans)
        entt::registry& Reg() { return m_Registry; }

        /**
         * @brief Starts the physics simulation, instantiates physics bodies for all entities with Rigidbody2DComponent
         */
        void StartPhysics();

        /**
         * @brief Stops the physics simulation and cleans up the physics world
         */
        void StopPhysics();

    private:
        entt::registry m_Registry;
        
        b2World* m_PhysicsWorld = nullptr;
        PhysicsContactListener* m_ContactListener = nullptr;

        friend class Entity;
    };

}
