#include "BSAEngine/Scene/Scene.h"
#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Scene/ScriptableEntity.h"
#include <glad/glad.h>

namespace BSA {

    Scene::Scene() {}

    Scene::~Scene() {}

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        
        // Her varlık varsayılan olarak bir konuma ve isme sahip olmalidir.
        entity.AddComponent<TransformComponent>();
        
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        if (entity.HasComponent<RelationshipComponent>()) {
            auto& children = entity.GetComponent<RelationshipComponent>().Children;
            // Iterate over a copy because recursive destruction might alter arrays
            std::vector<entt::entity> childrenCopy = children;
            for (auto childHandle : childrenCopy) {
                DestroyEntity(Entity{ childHandle, this });
            }
        }
        m_Registry.destroy(entity);
    }

    BSA::Math::Matrix4 Scene::GetWorldTransform(Entity entity) {
        BSA::Math::Matrix4 transform = entity.GetComponent<TransformComponent>().GetTransform();

        if (entity.HasComponent<RelationshipComponent>()) {
            auto parentHandle = entity.GetComponent<RelationshipComponent>().Parent;
            if (parentHandle != entt::null) {
                Entity parentEntity{ parentHandle, this };
                transform = GetWorldTransform(parentEntity) * transform;
            }
        }

        return transform;
    }

    void Scene::OnUpdate(float ts, std::shared_ptr<BSA::Shader> defaultShader, std::shared_ptr<BSA::VertexArray> defaultVAO) {
        
        // --- 1. SCRIPT ÇALIŞTIRMA MANTIĞI ---
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Script instantiator kontrolü (İlk çağrıldığında hafızada yarat)
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnUpdate(ts);
        });

        // --- 2. RENDER ÇİZİM (KAMERA VE EKRAN GÜNCELLEMESİ) ---
        BSA::PerspectiveCamera* mainCamera = nullptr;
        // İleride ana kameranın kendi Entity position'ı da hesaba katılacak: BSA::Math::Matrix4 cameraTransform;

        auto cameraView = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : cameraView) {
            auto [transform, camera] = cameraView.get<TransformComponent, CameraComponent>(entity);
            if (camera.Primary) {
                mainCamera = &camera.Camera;
                // Şimdilik kameranın sadece GetViewProjectionMatrix'ini kullanıyoruz, transform bilgisini 3B uzayda atlıyoruz
                break;
            }
        }

        if (mainCamera) {
            defaultShader->Bind();
            defaultShader->UploadUniformMat4("u_ViewProjection", mainCamera->GetViewProjectionMatrix());

            // Çizilebilir her Mesh varligini Transform matrisi eşliğinde çizdir
            auto meshView = m_Registry.view<TransformComponent, MeshRendererComponent>();
            for (auto entityHandle : meshView) {
                Entity entity{ entityHandle, this };

                defaultShader->UploadUniformMat4("u_Transform", GetWorldTransform(entity));
                defaultVAO->Bind();
                glDrawElements(GL_TRIANGLES, defaultVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

}
