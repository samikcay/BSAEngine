#pragma once

#include "BSAEngine/Math/Matrix4.h"
#include "BSAEngine/Math/Vector3.h"
#include "BSAEngine/Renderer/PerspectiveCamera.h"
#include "BSAEngine/Renderer/StaticMesh.h"
#include "BSAEngine/Renderer/Material.h"
#include "BSAEngine/Core/UUID.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace BSA {

    struct IDComponent {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };

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
        std::shared_ptr<StaticMesh> Mesh;
        std::shared_ptr<Material> Mat;

        MeshRendererComponent() = default;
        MeshRendererComponent(const std::shared_ptr<StaticMesh>& mesh) : Mesh(mesh) {}
        MeshRendererComponent(const std::shared_ptr<StaticMesh>& mesh, const std::shared_ptr<Material>& material) 
            : Mesh(mesh), Mat(material) {}
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
        bool FixedAspectRatio = false;

        float FOV = 45.0f; // Degrees
        float Near = 0.1f;
        float Far = 1000.0f;

        CameraComponent() : Camera(BSA::Math::Radians(45.0f), 1.778f, 0.1f, 1000.0f) {}
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

    struct ScriptComponent {
        std::string ClassName; // Ornek: "Sandbox.Player"
        bool Instantiated = false;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent&) = default;
        ScriptComponent(const std::string& className) : ClassName(className) {}
    };

    // ============================================
    // FİZİK COMPONENTLERİ
    // ============================================

    // Rigid Body Tipleri
    enum class BodyType {
        Static = 0,    // Hareket etmez (yer, duvar vb.)
        Dynamic,       // Fizik kuvvetlerine tepki verir (oyuncu, mermi vb.)
        Kinematic      // Programatik hareket eder, fizikten etkilenmez ama başkalarını iter
    };

    struct Rigidbody2DComponent {
        BodyType Type = BodyType::Dynamic;

        // Kütle ve yerçekimi
        float Mass = 1.0f;
        float GravityScale = 1.0f;

        // Hareket kısıtlamaları
        bool FixedRotation = false; // true ise fizik motoru bu cismi döndürmez

        // Başlangıç hızları (simülasyon başladığında uygulanır)
        BSA::Math::Vector3 LinearVelocity = { 0.0f, 0.0f, 0.0f };
        BSA::Math::Vector3 AngularVelocity = { 0.0f, 0.0f, 0.0f };

        // Runtime: Fizik motorunun iç body pointer'ı (Box2D b2Body* veya Jolt BodyID)
        // Bu alan serialize edilmez, sadece çalışma zamanında kullanılır
        void* RuntimeBody = nullptr;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
    };

    struct BoxCollider2DComponent {
        // Yarı boyutlar (half-extents): Kutunun merkeze göre X, Y, Z uzantıları
        BSA::Math::Vector3 HalfExtents = { 0.5f, 0.5f, 0.5f };

        // Entity merkezine göre collider ofseti
        BSA::Math::Vector3 Offset = { 0.0f, 0.0f, 0.0f };

        // Fiziksel malzeme özellikleri
        float Density = 1.0f;       // Yoğunluk (kütle hesabını etkiler)
        float Friction = 0.5f;      // Sürtünme katsayısı (0 = buz, 1 = kauçuk)
        float Restitution = 0.0f;   // Sekme katsayısı (0 = sönümlü, 1 = tam elastik)

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };

    struct CircleCollider2DComponent {
        // Küre yarıçapı
        float Radius = 0.5f;

        // Entity merkezine göre collider ofseti
        BSA::Math::Vector3 Offset = { 0.0f, 0.0f, 0.0f };

        // Fiziksel malzeme özellikleri
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };

    // ============================================
    // IŞIK COMPONENTLERİ
    // ============================================

    struct DirectionalLightComponent {
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;

        DirectionalLightComponent() = default;
        DirectionalLightComponent(const DirectionalLightComponent&) = default;
    };

    struct PointLightComponent {
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;
        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;

        PointLightComponent() = default;
        PointLightComponent(const PointLightComponent&) = default;
    };

    struct SpotLightComponent {
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;
        float CutOff = 12.5f;      // Derece cinsinden
        float OuterCutOff = 15.0f; // Derece cinsinden

        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;

        SpotLightComponent() = default;
        SpotLightComponent(const SpotLightComponent&) = default;
    };

}
