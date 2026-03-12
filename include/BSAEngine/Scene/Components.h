#pragma once

#include "BSAEngine/Math/Matrix4.h"
#include "BSAEngine/Math/Vector3.h"
#include "BSAEngine/Renderer/PerspectiveCamera.h"
#include <string>
#include <vector>

namespace BSA {

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };

    struct TransformComponent {
        BSA::Math::Vector3 Translation = { 0.0f, 0.0f, 0.0f };
        BSA::Math::Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
        BSA::Math::Vector3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const BSA::Math::Vector3& translation)
            : Translation(translation) {}

        BSA::Math::Matrix4 GetTransform() const {
            BSA::Math::Matrix4 rotation = BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), Rotation.x, { 1.0f, 0.0f, 0.0f })
                                        * BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), Rotation.y, { 0.0f, 1.0f, 0.0f })
                                        * BSA::Math::Matrix4::Rotate(BSA::Math::Matrix4(1.0f), Rotation.z, { 0.0f, 0.0f, 1.0f });

            return BSA::Math::Matrix4::Translate(BSA::Math::Matrix4(1.0f), Translation)
                 * rotation
                 * BSA::Math::Matrix4::Scale(BSA::Math::Matrix4(1.0f), Scale);
        }
    };

    struct MeshRendererComponent {
        // İleride buraya Mesh objesi (VBO/IBO baglantilari) ve Material eklenecek.
        // Şimdilik sadece varlığına dair bir "flag (etiket)" görevi görerek
        // Sandbox içindeki render motorunu tetikleyecek.
        int dummy = 0;

        MeshRendererComponent() = default;
    };

    struct RelationshipComponent {
        entt::entity Parent = entt::null;
        std::vector<entt::entity> Children;

        RelationshipComponent() = default;
        RelationshipComponent(const RelationshipComponent&) = default;
    };

    struct CameraComponent {
        BSA::PerspectiveCamera Camera;
        bool Primary = true; // Ana kameramiz mi? (Birden fazla kamera olursa isimize yariyor)

        CameraComponent() : Camera(BSA::Math::Radians(45.0f), 1.778f, 0.1f, 100.0f) {}
        CameraComponent(const CameraComponent&) = default;
    };

    // Forward declaration of ScriptableEntity to avoid circular dependency
    class ScriptableEntity;

    struct NativeScriptComponent {
        ScriptableEntity* Instance = nullptr;

        ScriptableEntity* (*InstantiateScript)();
        void (*DestroyScript)(NativeScriptComponent*);

        template<typename T>
        void Bind() {
            InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
            DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };

}
