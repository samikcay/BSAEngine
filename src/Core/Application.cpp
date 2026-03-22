#include "BSAEngine/Core/Application.h"
#include "BSAEngine/ImGui/ImGuiLayer.h"
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
#include "BSAEngine/Panels/SceneHierarchyPanel.h"
#include "BSAEngine/Panels/ContentBrowserPanel.h" // Added this include
#include "BSAEngine/Scripting/ScriptEngine.h" // Added this include
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

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

        // ImGui katmanini yarat ve baslat
        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init();
    }

    Application::~Application() {
        BSA_ENGINE_INFO("Sandbox Uygulamasi (Oyun) Kapatiliyor...");
        m_ImGuiLayer->Shutdown();
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




        // ---------- 3. KISIM: FRAMEBUFFER (OFF-SCREEN HEDEFİ) ----------
        BSA::FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        std::shared_ptr<BSA::Framebuffer> framebuffer(BSA::Framebuffer::Create(fbSpec));

        // ---------- C# SCRIPT ENGINE BAŞLATMA ----------
        BSA::ScriptEngine::Init();
        // İleride bu dll oyun projesinden (ör: Sandbox/bin) yüklenecek
        BSA::ScriptEngine::LoadAssembly("bin/Debug/Debug/BSA-ScriptCore.dll");
        BSA::ScriptEngine::PrintAssemblyTypes();

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

        // Scene Hierarchy ve Inspector panelini yarat
        BSA::SceneHierarchyPanel sceneHierarchyPanel(activeScene);
        BSA::ContentBrowserPanel contentBrowserPanel;

        // Viewport paneli boyut takibi
        ImVec2 viewportSize = { (float)fbSpec.Width, (float)fbSpec.Height };

        // Penceremizi temsil eden geçici loop (Engine yapısı tam oturana kadar)
        GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());

        float lastFrameTime = 0.0f;
        float fixedTimeStep = 1.0f / 60.0f; // 60 Hz fizik güncellemesi
        float accumulator = 0.0f;

        // --- FİZİK MOTORUNU BAŞLAT ---
        activeScene->StartPhysics();

        while (m_Running) {
            float time = (float)glfwGetTime();
            float timestep = time - lastFrameTime;
            lastFrameTime = time;

            // Maksimum frame gecikmesi siniri (spiral of death onleyici)
            if (timestep > 0.25f)
                timestep = 0.25f;

            accumulator += timestep;

            if (glfwWindowShouldClose(window))
                m_Running = false;

            // --- FİZİK GÜNCELLEMESİ (FIXED UPDATE) ---
            while (accumulator >= fixedTimeStep) {
                activeScene->OnFixedUpdate(fixedTimeStep);
                accumulator -= fixedTimeStep;
            }

            // --- AŞAMA 1: RENDER TO FRAMEBUFFER ---
            framebuffer->Bind();
            glEnable(GL_DEPTH_TEST);
            
            // Arka plan rengi (Koyu gri) ve Derinlik Temizleme (FBO Üzerinde)
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

            // Sahnenin (Scene) kendini isletmesine (Update etmesine) izin ver.
            // Butun rendering (Gorsellestirme) ve Script cagirimlari bu loopun icerisindedir!
            activeScene->OnUpdate(timestep, cubeShader, cubeVertexArray);
            
            // --- AŞAMA 2: FRAMEBUFFER'I BIRAK, EKRANI TEMİZLE ---
            framebuffer->Unbind();
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // --- AŞAMA 3: IMGUI + DOCKSPACE ---
            m_ImGuiLayer->Begin();

            // Dockspace ayarları
            static bool dockspaceOpen = true;
            static bool opt_fullscreen_persistant = true;
            bool opt_fullscreen = opt_fullscreen_persistant;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            if (opt_fullscreen) {
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            // Dockspace arka planını gizlemek istersen ImGuiWindowFlags_NoBackground ekleyebilirsin
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("BSAEngine DockSpace", &dockspaceOpen, window_flags);
            ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Dockspace'in aktif olmadığı durumlarda ImGui demo penceresi gibi klasik pencere mantığı ile devam eder
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("BSAEngineDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            // Üst menü bar
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Exit")) {
                        Close();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // --- SCENE HIERARCHY & INSPECTOR ---
            sceneHierarchyPanel.OnImGuiRender();

            // --- CONTENT BROWSER ---
            contentBrowserPanel.OnImGuiRender();

            // --- VIEWPORT PANELİ ---
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
            ImGui::Begin("Viewport");

            // Panel boyutu değiştiyse framebuffer'ı yeniden oluştur
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            if (viewportSize.x != viewportPanelSize.x || viewportSize.y != viewportPanelSize.y) {
                viewportSize = viewportPanelSize;
                framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            }

            // Framebuffer texture'ünü Viewport paneline çiz
            uint32_t textureID = framebuffer->GetColorAttachmentRendererID();
            ImGui::Image((ImTextureID)(uint64_t)textureID, viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

            ImGui::End();
            ImGui::PopStyleVar();

            ImGui::End(); // Dockspace ana penceresi

            m_ImGuiLayer->End();

            // Cift Buffer temizligi vb.
            m_Window->OnUpdate();
        }

        // --- FİZİK MOTORUNU DURDUR ---
        activeScene->StopPhysics();

        // --- C# MONO MOTORUNU KAPAT ---
        BSA::ScriptEngine::Shutdown();
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

        m_ImGuiLayer->OnEvent(e);

        // Test amaçlı tüm Eventleri bas (İlerde kaldırılacak)
        // BSA_ENGINE_TRACE("{0}", e.ToString());
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

} // namespace BSA
