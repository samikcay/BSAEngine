#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Events/ApplicationEvent.h"
#include "BSAEngine/Renderer/Buffer.h"
#include "BSAEngine/Renderer/VertexArray.h"
#include "BSAEngine/Renderer/Shader.h"
#include "BSAEngine/Renderer/PerspectiveCamera.h"
#include "BSAEngine/Renderer/Framebuffer.h"
#include "BSAEngine/Scene/Scene.h"
#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Scene/ScriptableEntity.h"
#include "BSAEngine/Math/Math.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BSA {

    Application* Application::s_Instance = nullptr;

    // --- GEÇİCİ SANDBOX NATIVE SCRIPT ORNEGI ---
    // Küpü kendi ekseni etrafinda donduren oyun davranisi (Script)
    class CubeController : public BSA::ScriptableEntity {
    public:
        void OnCreate() override {
            BSA_ENGINE_INFO("CubeController isimli C++ Script'i sahnede calistirilmaya basladi!");
        }

        void OnDestroy() override {
            BSA_ENGINE_INFO("CubeController silindi!");
        }

        void OnUpdate(float ts) override {
            // Scriptin baglandigi entity'nin bilesenine (Component) ulas!
            auto& transformComp = GetComponent<BSA::TransformComponent>();
            transformComp.Rotation.x += (ts * 0.5f);
            transformComp.Rotation.y += (ts * 1.0f);
            transformComp.Rotation.z += (ts * 0.2f);
        }
    };


    Application::Application() {
        if (s_Instance) {
            BSA_ENGINE_ERROR("Application zaten olusturulmus!");
            return;
        }
        s_Instance = this;

        BSA_ENGINE_INFO("Sandbox Uygulamasi (Oyun) Olusturuldu!");

        // Varsayılan pencere yaratılıyor
        m_Window = std::unique_ptr<Window>(Window::Create());
        
        // Window'dan gelen eventleri (OnEvent) fonksiyonuna bagla
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    }

    Application::~Application() {
        BSA_ENGINE_INFO("Sandbox Uygulamasi (Oyun) Kapatiliyor...");
    }

    void Application::Run() {
        BSA_ENGINE_INFO("Oyun Dongusu (Game Loop) Basladi.");

        // ---------- 1. KISIM: 3D KÜP (OFF-SCREEN ÇİZİLECEK) ----------
        // VAO (Vertex Array Object) Olustur
        std::shared_ptr<BSA::VertexArray> cubeVertexArray(BSA::VertexArray::Create());

        // 3D Küp Vertex Verileri (Pozisyon ve Renk)
        float cubeVertices[] = {
            // Position (X, Y, Z)     // Color (R, G, B, A)
            -0.5f, -0.5f,  0.5f,      1.0f, 0.0f, 0.0f, 1.0f, // 0: On-Sol-Alt (Kırmızı)
             0.5f, -0.5f,  0.5f,      0.0f, 1.0f, 0.0f, 1.0f, // 1: On-Sag-Alt (Yeşil)
             0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 1.0f, 1.0f, // 2: On-Sag-Ust (Mavi)
            -0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 0.0f, 1.0f, // 3: On-Sol-Ust (Sarı)
            -0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 1.0f, 1.0f, // 4: Arka-Sol-Alt (Magenta)
             0.5f, -0.5f, -0.5f,      0.0f, 1.0f, 1.0f, 1.0f, // 5: Arka-Sag-Alt (Cyan)
             0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 1.0f, 1.0f, // 6: Arka-Sag-Ust (Beyaz)
            -0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 0.0f, 1.0f  // 7: Arka-Sol-Ust (Siyah)
        };

        std::shared_ptr<BSA::VertexBuffer> cubeVB(BSA::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));
        cubeVB->SetLayout({
            { BSA::ShaderDataType::Float3, "a_Position" },
            { BSA::ShaderDataType::Float4, "a_Color" }
        });
        cubeVertexArray->AddVertexBuffer(cubeVB);

        uint32_t cubeIndices[] = {
            0, 1, 2,  2, 3, 0, // Ön yüz
            1, 5, 6,  6, 2, 1, // Sağ yüz
            5, 4, 7,  7, 6, 5, // Arka yüz
            4, 0, 3,  3, 7, 4, // Sol yüz
            3, 2, 6,  6, 7, 3, // Üst yüz
            4, 5, 1,  1, 0, 4  // Alt yüz
        };
        std::shared_ptr<BSA::IndexBuffer> cubeIB(BSA::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));
        cubeVertexArray->SetIndexBuffer(cubeIB);

        std::string cubeVertexSrc = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
            out vec4 v_Color;
            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        std::string cubeFragmentSrc = R"(
            #version 330 core
            layout(location = 0) out vec4 color;
            in vec4 v_Color;
            void main() {
                color = v_Color;
            }
        )";

        std::shared_ptr<BSA::Shader> cubeShader(BSA::Shader::Create(cubeVertexSrc, cubeFragmentSrc));

        // ---------- 2. KISIM: EKRAN QUAD (POST-PROCESS) ----------
        std::shared_ptr<BSA::VertexArray> screenVertexArray(BSA::VertexArray::Create());
        
        float screenQuadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        std::shared_ptr<BSA::VertexBuffer> screenVB(BSA::VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices)));
        screenVB->SetLayout({
            { BSA::ShaderDataType::Float2, "a_Position" },
            { BSA::ShaderDataType::Float2, "a_TexCoord" }
        });
        screenVertexArray->AddVertexBuffer(screenVB);

        uint32_t screenQuadIndices[] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<BSA::IndexBuffer> screenIB(BSA::IndexBuffer::Create(screenQuadIndices, sizeof(screenQuadIndices) / sizeof(uint32_t)));
        screenVertexArray->SetIndexBuffer(screenIB);

        std::string screenVertexSrc = R"(
            #version 330 core
            layout(location = 0) in vec2 a_Position;
            layout(location = 1) in vec2 a_TexCoord;
            out vec2 v_TexCoord;
            void main() {
                v_TexCoord = a_TexCoord;
                gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
            }
        )";

        std::string screenFragmentSrc = R"(
            #version 330 core
            layout(location = 0) out vec4 FragColor;
            in vec2 v_TexCoord;
            uniform sampler2D u_ScreenTexture;
            void main() {
                // Post-process shader: Basit Texture pass (Ileride blur, renk tersine çevirme vs. eklenebilir)
                vec3 col = texture(u_ScreenTexture, v_TexCoord).rgb;
                FragColor = vec4(col, 1.0);
            }
        )";

        std::shared_ptr<BSA::Shader> screenShader(BSA::Shader::Create(screenVertexSrc, screenFragmentSrc));


        // ---------- 3. KISIM: FRAMEBUFFER (OFF-SCREEN HEDEFİ) ----------
        BSA::FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        std::shared_ptr<BSA::Framebuffer> framebuffer(BSA::Framebuffer::Create(fbSpec));

        // ---------- 4. KISIM: ECS SAHNE KURULUMU ----------
        std::shared_ptr<BSA::Scene> activeScene = std::make_shared<BSA::Scene>();
        
        // "Colorful Cube" isimli bir varlık yarat ve onu oluştur
        BSA::Entity cubeEntity = activeScene->CreateEntity("Colorful Cube");
        cubeEntity.AddComponent<BSA::MeshRendererComponent>(); // Çizilebilir (Renderlanabilir) yap!
        cubeEntity.AddComponent<BSA::NativeScriptComponent>().Bind<CubeController>(); // C++ Script'i bağla!

        // "Child Cube" isimli uydu varlık yarat (Güneşin etrafında dönen dünya gibi)
        BSA::Entity childCube = activeScene->CreateEntity("Child Cube");
        childCube.AddComponent<BSA::MeshRendererComponent>();
        auto& childTransform = childCube.GetComponent<BSA::TransformComponent>();
        childTransform.Translation = { 2.0f, 0.0f, 0.0f }; // Ana küpten 2 birim sağda
        childTransform.Scale = { 0.5f, 0.5f, 0.5f }; // Yarı boyutta
        
        // Ebeveyne (Colorful Cube) bağla!
        childCube.SetParent(cubeEntity);

        // "Main Camera" isimli yeni bir varlık yarat
        BSA::Entity cameraEntity = activeScene->CreateEntity("Main Camera");
        cameraEntity.AddComponent<BSA::CameraComponent>();
        // Kamerayi uzayda geriye pozisyonluyoruz
        cameraEntity.GetComponent<BSA::CameraComponent>().Camera.SetPosition(BSA::Math::Vector3(0.0f, 0.0f, 3.0f));

        // Penceremizi temsil eden geçici loop (Engine yapısı tam oturana kadar)
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());

        while (m_Running) {
            if (glfwWindowShouldClose(window))
                m_Running = false;

            // --- AŞAMA 1: RENDER TO FRAMEBUFFER ---
            framebuffer->Bind();
            glEnable(GL_DEPTH_TEST);
            
            // Arka plan rengi (Koyu gri) ve Derinlik Temizleme (FBO Üzerinde)
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

            // Sahnenin (Scene) kendini isletmesine (Update etmesine) izin ver.
            // Butun rendering (Gorsellestirme) ve Script cagirimlari bu loopun icerisindedir!
            activeScene->OnUpdate(0.015f, cubeShader, cubeVertexArray);
            
            // --- AŞAMA 2: RENDER TO DEFAULT FRAMEBUFFER (SCREEN) ---
            framebuffer->Unbind(); // Varsayılan pencereye geri dön
            glDisable(GL_DEPTH_TEST); // Ekrana cizecegimiz Quad icin depth testing kapatiliyor
            
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Screen temizleme (beyaz)
            glClear(GL_COLOR_BUFFER_BIT);

            screenShader->Bind();
            // 0 Texture slotuna FBO'nun Color Attachmentini gonder
            glBindTexture(GL_TEXTURE_2D, framebuffer->GetColorAttachmentRendererID());
            
            screenVertexArray->Bind();
            glDrawElements(GL_TRIANGLES, screenVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);


            // Cift Buffer temizligi vb.
            m_Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

        // Test amaçlı tüm Eventleri bas (İlerde kaldırılacak)
        // BSA_ENGINE_TRACE("{0}", e.ToString());
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

} // namespace BSA
