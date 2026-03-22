#include "BSAEngine/Scene/Scene.h"
#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Scene/ScriptableEntity.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Scripting/ScriptEngine.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_contact.h>

namespace BSA {

    // ========================================================
    // Box2D Contact Listener (Çarpışma Dinleyicisi)
    // ========================================================
    class PhysicsContactListener : public b2ContactListener {
    public:
        void BeginContact(b2Contact* contact) override {
            b2Fixture* fixtureA = contact->GetFixtureA();
            b2Fixture* fixtureB = contact->GetFixtureB();

            // Box2D'ye "User Data" olarak ECS Entity Handle'larını kaydettiğimiz varsayılıyor
            // (Aşağıda b2BodyUserData olarak bunu atayacağız)
            entt::entity e1 = (entt::entity)fixtureA->GetBody()->GetUserData().pointer;
            entt::entity e2 = (entt::entity)fixtureB->GetBody()->GetUserData().pointer;

            // İleride bu iki Entity'nin birbirine çarptığını Native Script'lere iletebiliriz
            // Örnek:
            // if (scene->Reg().valid(e1) && scene->Reg().all_of<NativeScriptComponent>(e1)) {
            //      scene->Reg().get<NativeScriptComponent>(e1).Instance->OnCollisionBegin(Entity{e2, scene});
            // }
        }

        void EndContact(b2Contact* contact) override {
            // İleride Cikis anini scriptlere ilet
        }
    };

    Scene::Scene() {}

    Scene::~Scene() {
        StopPhysics();
    }

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

    void Scene::StartPhysics() {
        m_PhysicsWorld = new b2World({ 0.0f, -9.81f });
        m_ContactListener = new PhysicsContactListener();
        m_PhysicsWorld->SetContactListener(m_ContactListener);

        auto view = m_Registry.view<RigidbodyComponent>();
        for (auto entityHandle : view) {
            Entity entity = { entityHandle, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb = entity.GetComponent<RigidbodyComponent>();

            b2BodyDef bodyDef;
            switch (rb.Type) {
                case BodyType::Static:    bodyDef.type = b2_staticBody; break;
                case BodyType::Dynamic:   bodyDef.type = b2_dynamicBody; break;
                case BodyType::Kinematic: bodyDef.type = b2_kinematicBody; break;
            }

            // Box2D 2D pozisyon kabul eder, biz X ve Y eksenini veriyoruz
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z; // 2D düzlemde Z eksenindeki rotasyon
            bodyDef.fixedRotation = rb.FixedRotation;
            bodyDef.gravityScale = rb.GravityScale;

            bodyDef.userData.pointer = (uintptr_t)entityHandle;

            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            rb.RuntimeBody = body;

            // Doğrusal ve Açısal başlangıç hızları
            body->SetLinearVelocity({ rb.LinearVelocity.x, rb.LinearVelocity.y });
            body->SetAngularVelocity(rb.AngularVelocity.z);

            // --- BOX COLLIDER 2D ---
            if (entity.HasComponent<BoxColliderComponent>()) {
                auto& bc = entity.GetComponent<BoxColliderComponent>();

                b2PolygonShape boxShape;
                boxShape.SetAsBox(
                    bc.HalfExtents.x * transform.Scale.x, 
                    bc.HalfExtents.y * transform.Scale.y,
                    b2Vec2(bc.Offset.x, bc.Offset.y),
                    0.0f // Local rotasyon şimdilik 0
                );

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc.Density;
                fixtureDef.friction = bc.Friction;
                fixtureDef.restitution = bc.Restitution; // Bounciness

                // Eger agirlik degeri Box2D'nin shape density'sinden degil de doğrudan Mass degiskeninden alınmak isteniyorsa
                // b2MassData kullanılabilir.

                body->CreateFixture(&fixtureDef);
            }

            // --- SPHERE (CIRCLE) COLLIDER 2D ---
            if (entity.HasComponent<SphereColliderComponent>()) {
                auto& sc = entity.GetComponent<SphereColliderComponent>();

                b2CircleShape circleShape;
                circleShape.m_p.Set(sc.Offset.x, sc.Offset.y);
                circleShape.m_radius = sc.Radius * transform.Scale.x; // Scale.x'i alıyoruz varsayarak

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &circleShape;
                fixtureDef.density = sc.Density;
                fixtureDef.friction = sc.Friction;
                fixtureDef.restitution = sc.Restitution;

                body->CreateFixture(&fixtureDef);
            }
        }
    }

    void Scene::StopPhysics() {
        if (m_PhysicsWorld) {
            delete m_PhysicsWorld;
            m_PhysicsWorld = nullptr;
        }

        if (m_ContactListener) {
            delete m_ContactListener;
            m_ContactListener = nullptr;
        }
    }

    void Scene::OnFixedUpdate(float fixedTimeStep) {
        if (!m_PhysicsWorld) return;

        // Box2D standard step values (Genellikle hiz: 8, pozisyon: 3 iterasyon kullanilir)
        const int32_t velocityIterations = 8;
        const int32_t positionIterations = 3;

        m_PhysicsWorld->Step(fixedTimeStep, velocityIterations, positionIterations);

        // Fizik motorundan yeni pozisyonlari cekip ECS bilesenlerine yazalim
        auto view = m_Registry.view<RigidbodyComponent, TransformComponent>();
        for (auto entityHandle : view) {
            auto [rb, transform] = view.get<RigidbodyComponent, TransformComponent>(entityHandle);
            
            if (rb.Type == BodyType::Static)
                continue;

            b2Body* body = (b2Body*)rb.RuntimeBody;
            const b2Vec2& position = body->GetPosition();
            float angle = body->GetAngle();

            transform.Translation.x = position.x;
            transform.Translation.y = position.y;
            // Z eksenini ellemeyiz cünkü 2D fizik sadece XY düzlemindedir.
            
            transform.Rotation.z = angle;
        }
    }

    void Scene::OnUpdate(float ts, std::shared_ptr<BSA::Shader> defaultShader, std::shared_ptr<BSA::VertexArray> defaultVAO) {
        
        // --- 1. SCRIPT ÇALIŞTIRMA MANTIĞI ---
        m_Registry.view<NativeScriptComponent>().each([=](auto entityHandle, auto& nsc) {
            // Script instantiator kontrolü (İlk çağrıldığında hafızada yarat)
            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entityHandle, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnUpdate(ts);
        });

        // --- 1.5 C# (MONO) SCRIPT ÇALIŞTIRMA MANTIĞI ---
        ScriptEngine::SetSceneContext(this);
        auto scriptView = m_Registry.view<ScriptComponent>();
        for (auto entityHandle : scriptView) {
            Entity entity { entityHandle, this };
            auto& sc = entity.GetComponent<ScriptComponent>();
            if (!sc.ClassName.empty() && !sc.Instantiated) {
                ScriptEngine::OnCreateEntity(entity);
                sc.Instantiated = true;
            }
            if (sc.Instantiated) {
                ScriptEngine::OnUpdateEntity(entity, ts);
            }
        }

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
