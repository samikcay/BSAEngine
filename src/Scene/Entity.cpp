#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include <algorithm>
#include <vector>

namespace BSA {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene) {
    }

    void Entity::SetParent(Entity parent) {
        if (!HasComponent<RelationshipComponent>())
            AddComponent<RelationshipComponent>();
        if (!parent.HasComponent<RelationshipComponent>())
            parent.AddComponent<RelationshipComponent>();

        auto& ourRelationship = GetComponent<RelationshipComponent>();
        
        // Remove from old parent
        if (ourRelationship.Parent != entt::null) {
            Entity oldParent = Entity{ ourRelationship.Parent, m_Scene };
            auto& oldParentChildren = oldParent.GetComponent<RelationshipComponent>().Children;
            oldParentChildren.erase(std::remove(oldParentChildren.begin(), oldParentChildren.end(), m_EntityHandle), oldParentChildren.end());
        }

        // Add to new parent
        ourRelationship.Parent = parent.m_EntityHandle;
        auto& newParentChildren = parent.GetComponent<RelationshipComponent>().Children;
        newParentChildren.push_back(m_EntityHandle);
    }

    Entity Entity::GetParent() {
        if (!HasComponent<RelationshipComponent>()) return Entity{};
        return Entity{ GetComponent<RelationshipComponent>().Parent, m_Scene };
    }

    bool Entity::HasParent() {
        if (!HasComponent<RelationshipComponent>()) return false;
        return GetComponent<RelationshipComponent>().Parent != entt::null;
    }

    std::vector<Entity> Entity::GetChildren() {
        if (!HasComponent<RelationshipComponent>()) return {};
        
        std::vector<Entity> childrenOut;
        auto& childrenIn = GetComponent<RelationshipComponent>().Children;
        for (auto childId : childrenIn) {
            childrenOut.push_back({ childId, m_Scene });
        }
        return childrenOut;
    }

}
