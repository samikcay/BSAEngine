#pragma once

#include "BSAEngine/Scene/Scene.h"
#include <entt/entt.hpp>
#include <cstdint>
#include "BSAEngine/Log/Log.h"
#include <vector> // Added for std::vector

namespace BSA {

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;

        // Bileşen ekleme
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            if (HasComponent<T>()) {
                BSA_ENGINE_WARN("Entity already has component!");
                return GetComponent<T>();
            }
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            return component;
        }

        // Bileşen alma
        template<typename T>
        T& GetComponent() {
            if (!HasComponent<T>()) {
                BSA_ENGINE_WARN("Entity does not have component!");
            }
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        // Bileşen kontrolü
        template<typename T>
        bool HasComponent() const {
            return m_Scene->m_Registry.try_get<T>(m_EntityHandle) != nullptr;
        }

        // Bileşen silme
        template<typename T>
        void RemoveComponent() {
            if (!HasComponent<T>()) {
                BSA_ENGINE_WARN("Entity does not have component to remove!");
                return;
            }
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        // Hiyerarşi (Scene Graph)
        void SetParent(Entity parent);
        Entity GetParent();
        bool HasParent();
        std::vector<Entity> GetChildren();

        // Geçerlilik kontrolleri
        operator bool() const { return m_EntityHandle != entt::null; }
        operator entt::entity() const { return m_EntityHandle; }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        bool operator==(const Entity& other) const {
            return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
        }
        bool operator!=(const Entity& other) const {
            return !(*this == other);
        }

    private:
        entt::entity m_EntityHandle = entt::null;
        Scene* m_Scene = nullptr;
    };

}
