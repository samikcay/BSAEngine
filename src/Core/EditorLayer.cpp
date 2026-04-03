#include "BSAEngine/Core/EditorLayer.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Scene/Entity.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Scene/ScriptableEntity.h"
#include "BSAEngine/Math/Math.h"
#include "BSAEngine/Scripting/ScriptEngine.h"
#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Renderer/RenderCommand.h"
#include "BSAEngine/Renderer/Renderer.h"
#include "BSAEngine/Scene/SceneSerializer.h"
#include "BSAEngine/Scene/PrefabSerializer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace BSA {

    // --- GEÇİCİ SANDBOX NATIVE SCRIPT ORNEGI ---
    class CubeController : public BSA::ScriptableEntity {
    public:
        void OnCreate() override {
            BSA_ENGINE_INFO("CubeController isimli C++ Script'i sahnede calistirilmaya basladi!");
        }

        void OnDestroy() override {
            BSA_ENGINE_INFO("CubeController silindi!");
        }

        void OnUpdate(float ts) override {
            auto& transformComp = GetComponent<BSA::TransformComponent>();
            transformComp.Rotation.x += (ts * 0.5f);
            transformComp.Rotation.y += (ts * 1.0f);
            transformComp.Rotation.z += (ts * 0.2f);
        }
    };

    EditorLayer::EditorLayer()
        : Layer("EditorLayer")
    {
    }

    void EditorLayer::OnAttach() {
        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
        
        m_CubeMesh = StaticMesh::CreateCube();

        std::string cubeVertexSrc = R"(
            #version 450 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Normal;
            layout(location = 2) in vec2 a_TexCoord;

            layout(std140, binding = 0) uniform CameraData {
                mat4 u_ViewProjection;
                vec3 u_ViewPos;
            };

            uniform mat4 u_Transform;

            out vec3 v_Normal;
            out vec3 v_FragPos;
            out vec2 v_TexCoord;

            void main() {
                v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
                v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
                v_TexCoord = a_TexCoord;
                gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
            }
        )";

        std::string cubeFragmentSrc = R"(
            #version 450 core
            layout(location = 0) out vec4 color;
            layout(location = 1) out int color2;

            struct DirLight {
                vec3 Direction;
                vec3 Color;
                float Intensity;
            };

            struct PointLight {
                vec3 Position;
                vec3 Color;
                float Intensity;
                float Constant;
                float Linear;
                float Quadratic;
            };

            layout(std140, binding = 0) uniform CameraData {
                mat4 u_ViewProjection;
                vec3 u_ViewPos;
            };

            layout(std140, binding = 1) uniform LightingData {
                DirLight u_DirLight;
                PointLight u_PointLights[8];
                int u_PointLightCount;
            };

            in vec3 v_Normal;
            in vec3 v_FragPos;
            in vec2 v_TexCoord;

            uniform int u_EntityID;

            vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir) {
                vec3 lightDir = normalize(-light.Direction);
                float diff = max(dot(normal, lightDir), 0.0);
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                
                vec3 ambient = 0.1 * light.Color;
                vec3 diffuse = diff * light.Color * light.Intensity;
                vec3 specular = 0.5 * spec * light.Color;
                return (ambient + diffuse + specular);
            }

            vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
                vec3 lightDir = normalize(light.Position - fragPos);
                float diff = max(dot(normal, lightDir), 0.0);
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                float distance = length(light.Position - fragPos);
                float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));    
                
                vec3 ambient = 0.1 * light.Color;
                vec3 diffuse = diff * light.Color * light.Intensity;
                vec3 specular = 0.5 * spec * light.Color;
                return (ambient + diffuse + specular) * attenuation;
            }

            void main() {
                vec3 norm = normalize(v_Normal);
                vec3 viewDir = normalize(u_ViewPos - v_FragPos);

                vec3 result = CalculateDirLight(u_DirLight, norm, viewDir);

                for(int i = 0; i < u_PointLightCount; i++)
                    result += CalculatePointLight(u_PointLights[i], norm, v_FragPos, viewDir);

                color = vec4(result, 1.0);
                color2 = u_EntityID;
            }
        )";

        m_CubeShader = Shader::Create(cubeVertexSrc, cubeFragmentSrc);
        m_CubeMaterial = std::make_shared<Material>(m_CubeShader, "Cube Material");

        // ---------- 3. KISIM: FRAMEBUFFER ----------
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = std::shared_ptr<Framebuffer>(Framebuffer::Create(fbSpec));

        // ---------- C# SCRIPT ENGINE ----------
        ScriptEngine::Init();
        ScriptEngine::LoadAssembly("bin/Debug/Debug/BSA-ScriptCore.dll");
        // ScriptEngine::PrintAssemblyTypes();

        // ---------- 4. KISIM: ECS SAHNE KURULUMU ----------
        m_ActiveScene = std::make_shared<Scene>();
        
        Entity cubeEntity = m_ActiveScene->CreateEntity("Colorful Cube");
        cubeEntity.AddComponent<MeshRendererComponent>(m_CubeMesh, m_CubeMaterial);
        cubeEntity.AddComponent<NativeScriptComponent>().Bind<CubeController>();

        Entity lightEntity = m_ActiveScene->CreateEntity("Directional Light");
        auto& light = lightEntity.AddComponent<DirectionalLightComponent>();
        light.Color = { 1.0f, 1.0f, 1.0f };
        light.Intensity = 1.0f;
        lightEntity.GetComponent<TransformComponent>().Rotation = { -0.5f, -0.5f, 0.0f };

        Entity childCube = m_ActiveScene->CreateEntity("Child Cube");
        childCube.AddComponent<MeshRendererComponent>(m_CubeMesh, m_CubeMaterial);
        auto& childTransform = childCube.GetComponent<TransformComponent>();
        childTransform.Translation = { 2.0f, 0.0f, 0.0f };
        childTransform.Scale = { 0.5f, 0.5f, 0.5f };
        childCube.SetParent(cubeEntity);

        Entity cameraEntity = m_ActiveScene->CreateEntity("Main Camera");
        cameraEntity.AddComponent<CameraComponent>();
        cameraEntity.GetComponent<CameraComponent>().Camera.SetPosition(Math::Vector3(0.0f, 0.0f, 3.0f));

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach() {
        if (m_SceneState == SceneState::Play)
            OnSceneStop();
        ScriptEngine::Shutdown();
    }

    void EditorLayer::OnScenePlay() {
        if (m_SceneState == SceneState::Play) return;

        // Save the current scene so we can restore on Stop
        m_EditorScene = m_ActiveScene;

        // TODO: Deep-copy scene for play mode (currently shares the same scene)
        // For now, we just start physics on the active scene
        m_ActiveScene->StartPhysics();
        m_SceneState = SceneState::Play;
        BSA_ENGINE_INFO("Scene Play started.");
    }

    void EditorLayer::OnScenePause() {
        if (m_SceneState != SceneState::Play) return;
        m_SceneState = SceneState::Pause;
        BSA_ENGINE_INFO("Scene Paused.");
    }

    void EditorLayer::OnSceneStop() {
        if (m_SceneState == SceneState::Edit) return;

        m_ActiveScene->StopPhysics();
        m_SceneState = SceneState::Edit;
        m_Accumulator = 0.0f;

        // Restore the editor scene
        if (m_EditorScene) {
            m_ActiveScene = m_EditorScene;
            m_EditorScene = nullptr;
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        }
        BSA_ENGINE_INFO("Scene Stopped. Restored to edit state.");
    }

    void EditorLayer::OnUpdate(float ts) {
        // Only run physics and scripts in Play mode
        if (m_SceneState == SceneState::Play) {
            float fixedTimeStep = 1.0f / 60.0f;
            m_Accumulator += ts;

            while (m_Accumulator >= fixedTimeStep) {
                m_ActiveScene->OnFixedUpdate(fixedTimeStep);
                m_Accumulator -= fixedTimeStep;
            }
        }

        m_EditorCamera.OnUpdate(ts);

        // Render
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.105f, 0.11f, 1.0f });
        RenderCommand::Clear();

        m_Framebuffer->ClearAttachment(1, -1);

        m_ActiveScene->OnUpdate(ts, m_EditorCamera.GetViewProjection(), m_EditorCamera.GetPosition(), m_CubeShader, m_CubeMesh->GetVertexArray());
        
        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender() {
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("BSAEngine DockSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("BSAEngineDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {}
                if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Reload Assembly", "Ctrl+R")) ScriptEngine::ReloadAssembly();
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Select All", "Ctrl+A")) {}
                if (ImGui::MenuItem("Deselect All")) { m_SceneHierarchyPanel.SetSelectedEntity({}); }
                ImGui::Separator();
                if (ImGui::MenuItem("Project Settings...")) {}
                if (ImGui::MenuItem("Preferences...")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("GameObject")) {
                if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N")) {
                    m_ActiveScene->CreateEntity("GameObject");
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("3D Object")) {
                    if (ImGui::MenuItem("Cube")) {
                        Entity e = m_ActiveScene->CreateEntity("Cube");
                        e.AddComponent<MeshRendererComponent>(m_CubeMesh, m_CubeMaterial);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Light")) {
                    if (ImGui::MenuItem("Directional Light")) {
                        Entity e = m_ActiveScene->CreateEntity("Directional Light");
                        e.AddComponent<DirectionalLightComponent>();
                    }
                    if (ImGui::MenuItem("Point Light")) {
                        Entity e = m_ActiveScene->CreateEntity("Point Light");
                        e.AddComponent<PointLightComponent>();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Camera")) {
                    Entity e = m_ActiveScene->CreateEntity("Camera");
                    e.AddComponent<CameraComponent>();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Component")) {
                if (ImGui::MenuItem("Mesh Renderer")) {}
                if (ImGui::MenuItem("Camera")) {}
                ImGui::Separator();
                if (ImGui::BeginMenu("Physics 2D")) {
                    if (ImGui::MenuItem("Rigidbody 2D")) {}
                    if (ImGui::MenuItem("Box Collider 2D")) {}
                    if (ImGui::MenuItem("Circle Collider 2D")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Script")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window")) {
                if (ImGui::MenuItem("Hierarchy")) {}
                if (ImGui::MenuItem("Inspector")) {}
                if (ImGui::MenuItem("Project")) {}
                if (ImGui::MenuItem("Console")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About BSAEngine")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        UI_Toolbar();
        UI_StatusBar();

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Scene");

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y) {
            m_ViewportSize = viewportPanelSize;
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((ImTextureID)(uint64_t)textureID, m_ViewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // Mouse Picking
        auto [mx, my] = ImGui::GetMousePos();
        mx -= ImGui::GetWindowPos().x;
        my -= ImGui::GetWindowPos().y;

        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
                int pixelData = m_Framebuffer->ReadPixel(1, mouseX, (int)m_ViewportSize.y - mouseY);
                if (pixelData != -1) {
                    Entity selectedEntity = { (entt::entity)pixelData, m_ActiveScene.get() };
                    m_SceneHierarchyPanel.SetSelectedEntity(selectedEntity);
                } else {
                    m_SceneHierarchyPanel.SetSelectedEntity({});
                }
            }
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const char* path = (const char*)payload->Data;
                std::filesystem::path filepath = path;
                if (filepath.extension() == ".bsaprefab") {
                    PrefabSerializer::Deserialize(filepath.string(), m_ActiveScene.get());
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End(); // Dockspace
    }

    void EditorLayer::UI_Toolbar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

        // Unity toolbar background color
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f });

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float toolbarHeight = ImGui::GetWindowHeight();
        float size = toolbarHeight - 8.0f;
        float halfWidth = ImGui::GetWindowContentRegionMax().x * 0.5f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Center the play/pause/stop buttons
        ImGui::SetCursorPosX(halfWidth - (size * 1.5f + 4.0f));

        // Play Button - Unity style with background tint when active
        {
            bool isPlaying = (m_SceneState == SceneState::Play);
            if (isPlaying) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.15f, 0.35f, 0.55f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.20f, 0.40f, 0.60f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.12f, 0.30f, 0.50f, 1.0f });
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.30f, 0.30f, 0.30f, 0.5f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.25f, 0.25f, 0.25f, 0.5f });
            }
            if (ImGui::Button("##play", ImVec2(size, size))) {
                if (m_SceneState == SceneState::Edit)
                    OnScenePlay();
                else if (m_SceneState == SceneState::Pause)
                    m_SceneState = SceneState::Play;
            }
            ImVec2 pMin = ImGui::GetItemRectMin();
            ImVec2 pMax = ImGui::GetItemRectMax();
            float cx = (pMin.x + pMax.x) * 0.5f;
            float cy = (pMin.y + pMax.y) * 0.5f;
            float r = size * 0.22f;
            ImU32 col = isPlaying ? IM_COL32(200, 220, 255, 255) : IM_COL32(200, 200, 200, 255);
            drawList->AddTriangleFilled({cx - r * 0.5f, cy - r}, {cx - r * 0.5f, cy + r}, {cx + r, cy}, col);
            ImGui::PopStyleColor(3);
        }

        ImGui::SameLine(0, 2);

        // Pause Button
        {
            bool isPaused = (m_SceneState == SceneState::Pause);
            if (isPaused) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.15f, 0.35f, 0.55f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.20f, 0.40f, 0.60f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.12f, 0.30f, 0.50f, 1.0f });
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.30f, 0.30f, 0.30f, 0.5f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.25f, 0.25f, 0.25f, 0.5f });
            }
            if (ImGui::Button("##pause", ImVec2(size, size))) {
                OnScenePause();
            }
            ImVec2 pMin = ImGui::GetItemRectMin(); ImVec2 pMax = ImGui::GetItemRectMax();
            float cx = (pMin.x + pMax.x) * 0.5f; float cy = (pMin.y + pMax.y) * 0.5f;
            float r = size * 0.22f;
            ImU32 col = isPaused ? IM_COL32(200, 220, 255, 255) : IM_COL32(200, 200, 200, 255);
            drawList->AddRectFilled({cx - r * 0.6f, cy - r}, {cx - r * 0.15f, cy + r}, col, 1.0f);
            drawList->AddRectFilled({cx + r * 0.15f, cy - r}, {cx + r * 0.6f, cy + r}, col, 1.0f);
            ImGui::PopStyleColor(3);
        }

        ImGui::SameLine(0, 2);

        // Stop Button
        {
            bool canStop = (m_SceneState != SceneState::Edit);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.30f, 0.30f, 0.30f, 0.5f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.25f, 0.25f, 0.25f, 0.5f });
            if (ImGui::Button("##stop", ImVec2(size, size))) {
                OnSceneStop();
            }
            ImVec2 pMin = ImGui::GetItemRectMin(); ImVec2 pMax = ImGui::GetItemRectMax();
            float cx = (pMin.x + pMax.x) * 0.5f; float cy = (pMin.y + pMax.y) * 0.5f;
            float r = size * 0.22f;
            ImU32 col = canStop ? IM_COL32(200, 200, 200, 255) : IM_COL32(100, 100, 100, 255);
            drawList->AddRectFilled({cx - r, cy - r}, {cx + r, cy + r}, col, 1.0f);
            ImGui::PopStyleColor(3);
        }

        ImGui::PopStyleColor(); // WindowBg
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    void EditorLayer::UI_StatusBar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 3));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f });
        ImGui::Begin("##statusbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Scene state indicator
        const char* stateText = "Edit Mode";
        ImVec4 stateColor = { 0.50f, 0.50f, 0.50f, 1.0f };
        if (m_SceneState == SceneState::Play) {
            stateText = "Playing";
            stateColor = { 0.2f, 0.7f, 0.2f, 1.0f };
        } else if (m_SceneState == SceneState::Pause) {
            stateText = "Paused";
            stateColor = { 0.8f, 0.7f, 0.2f, 1.0f };
        }
        ImGui::TextColored(stateColor, "%s", stateText);
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::TextDisabled("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::TextDisabled("OpenGL 4.5");

        float rightText = ImGui::CalcTextSize("BSAEngine v0.1.0").x;
        ImGui::SameLine(ImGui::GetWindowWidth() - rightText - 12);
        ImGui::TextDisabled("BSAEngine v0.1.0");

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }


    void EditorLayer::OnEvent(Event& e) {
        m_EditorCamera.OnEvent(e);
    }

} // namespace BSA
