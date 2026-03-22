#include "BSAEngine/Panels/SceneHierarchyPanel.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Log/Log.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <cstring>
#include <filesystem> // Added for drag and drop
#include <string>     // Added for drag and drop

namespace BSA {

    SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context) {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context) {
        m_Context = context;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        // ============================================
        // SCENE HIERARCHY PANELİ
        // ============================================
        ImGui::Begin("Scene Hierarchy");

        if (m_Context) {
            // Tum entity'leri iterate et, sadece kok (root) olanlari ciz
            m_Context->Reg().view<entt::entity>().each([&](auto entityID) {
                // Silinmis entity kontrolu
                if (!m_Context->Reg().valid(entityID))
                    return;

                Entity entity{ entityID, m_Context.get() };

                // Sadece parent'i olmayan (root) entity'leri goster
                // Child entity'ler, parent'larinin DrawEntityNode'u icinde recursive cizilecek
                if (entity.HasComponent<RelationshipComponent>()) {
                    auto& rel = entity.GetComponent<RelationshipComponent>();
                    if (rel.Parent != entt::null)
                        return; // Bu bir child, root listesinde atla
                }

                DrawEntityNode(entity);
            });

            // Bos alana tiklandiginda secimi kaldir
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_SelectionContext = {};

            // Sag-tik bos alan: Entity olusturma menusu
            if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                if (ImGui::MenuItem("Create Empty Entity"))
                    m_Context->CreateEntity("Empty Entity");
                ImGui::EndPopup();
            }
        }

        ImGui::End();

        // ============================================
        // PROPERTIES (INSPECTOR) PANELİ
        // ============================================
        ImGui::Begin("Properties");

        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        // Ağaç düğümü flag'leri
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        
        // Secili entity'yi highlight et
        if (m_SelectionContext == entity)
            flags |= ImGuiTreeNodeFlags_Selected;

        // Child'i olmayan entity'ler icin yaprak (leaf) dugumu kullan
        bool hasChildren = false;
        if (entity.HasComponent<RelationshipComponent>()) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            hasChildren = !rel.Children.empty();
        }
        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf;

        // TreeNode'u ciz (ID olarak entity handle kullan)
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());

        // Sol-tik ile sec
        if (ImGui::IsItemClicked()) {
            m_SelectionContext = entity;
        }

        // Sag-tik ile silme menusu
        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Entity")) {
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }

        // Eger agac dugumu acildiysa, child'lari recursive ciz
        if (opened) {
            if (entity.HasComponent<RelationshipComponent>()) {
                auto& rel = entity.GetComponent<RelationshipComponent>();
                for (auto childHandle : rel.Children) {
                    Entity childEntity{ childHandle, m_Context.get() };
                    DrawEntityNode(childEntity);
                }
            }
            ImGui::TreePop();
        }

        // Entity silme islemi (UI traverse bittikten sonra)
        if (entityDeleted) {
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
            m_Context->DestroyEntity(entity);
        }
    }

    // ============================================
    // HELPER: Vec3 drag control (Translation/Rotation/Scale icin)
    // ============================================
    static void DrawVec3Control(const std::string& label, BSA::Math::Vector3& values, float resetValue = 0.0f, float speed = 0.1f) {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100.0f);

        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float totalWidth = ImGui::CalcItemWidth();
        float itemWidth = (totalWidth - 2.0f * ImGui::GetStyle().ItemInnerSpacing.x) / 3.0f;

        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // --- X ---
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
        ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // --- Y ---
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
        ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // --- Z ---
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushItemWidth(itemWidth);
        ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity) {
        // ---- TAG COMPONENT (İsim) ----
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
#ifdef _MSC_VER
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), _TRUNCATE);
#else
            strncpy(buffer, tag.c_str(), sizeof(buffer) - 1);
#endif

            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }

        ImGui::Separator();

        // ---- TRANSFORM COMPONENT ----
        if (entity.HasComponent<TransformComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), 
                    ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
                
                auto& transform = entity.GetComponent<TransformComponent>();
                DrawVec3Control("Translation", transform.Translation);
                DrawVec3Control("Rotation", transform.Rotation);
                DrawVec3Control("Scale", transform.Scale, 1.0f);

                ImGui::TreePop();
            }
        }

        // ---- CAMERA COMPONENT ----
        if (entity.HasComponent<CameraComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {

                auto& cameraComp = entity.GetComponent<CameraComponent>();
                ImGui::Checkbox("Primary", &cameraComp.Primary);

                ImGui::TreePop();
            }
        }

        // ---- MESH RENDERER COMPONENT ----
        if (entity.HasComponent<MeshRendererComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(MeshRendererComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer")) {

                ImGui::Text("Mesh Renderer aktif");

                ImGui::TreePop();
            }
        }

        // ---- NATIVE SCRIPT COMPONENT ----
        if (entity.HasComponent<NativeScriptComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Native Script")) {

                auto& nsc = entity.GetComponent<NativeScriptComponent>();
                ImGui::Text("Script: %s", nsc.Instance ? "Aktif" : "Bekliyor");

                ImGui::TreePop();
            }
        }

        // ---- SCRIPT COMPONENT (C#) ----
        if (entity.HasComponent<ScriptComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(ScriptComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Script (C#)")) {

                auto& sc = entity.GetComponent<ScriptComponent>();
                
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
#ifdef _MSC_VER
                strncpy_s(buffer, sizeof(buffer), sc.ClassName.c_str(), _TRUNCATE);
#else
                strncpy(buffer, sc.ClassName.c_str(), sizeof(buffer) - 1);
#endif

                if (ImGui::InputText("Class", buffer, sizeof(buffer))) {
                    sc.ClassName = std::string(buffer);
                }

                // DRAG AND DROP TARGET (C# Script Ataması)
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                        const char* path = (const char*)payload->Data;
                        std::filesystem::path filepath = path;
                        
                        // Eger uzantisi .cs ise, sinif adi olarak atayalim (Örn: "Sandbox.Player")
                        if (filepath.extension() == ".cs") {
                            std::string filename = filepath.stem().string(); // "Player"
                            
                            // Tam isim (Sandbox namespace'i) basitlestirildi
                            // Burada kisa yol yapiyoruz: Eger gercek yapida farklı bir alan adı (namespace) varsa
                            // onu bulmak icin dosyanın içi okunabilir, fakat genelde "Sandbox.SınıfAdı" olur.
                            sc.ClassName = "Sandbox." + filename;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::TreePop();
            }
        }

        // ---- RELATIONSHIP COMPONENT ----
        if (entity.HasComponent<RelationshipComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(RelationshipComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Relationship")) {

                auto& rel = entity.GetComponent<RelationshipComponent>();
                
                if (rel.Parent != entt::null) {
                    Entity parentEntity{ rel.Parent, m_Context.get() };
                    auto& parentTag = parentEntity.GetComponent<TagComponent>().Tag;
                    ImGui::Text("Parent: %s", parentTag.c_str());
                } else {
                    ImGui::Text("Parent: None (Root)");
                }

                ImGui::Text("Children: %d", (int)rel.Children.size());

                ImGui::TreePop();
            }
        }

        // ---- RIGIDBODY COMPONENT ----
        if (entity.HasComponent<RigidbodyComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(RigidbodyComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Rigidbody")) {

                auto& rb = entity.GetComponent<RigidbodyComponent>();

                // Body Type combo
                const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
                int currentType = (int)rb.Type;
                if (ImGui::Combo("Body Type", &currentType, bodyTypeStrings, 3)) {
                    rb.Type = (BodyType)currentType;
                }

                ImGui::DragFloat("Mass", &rb.Mass, 0.1f, 0.001f, 1000.0f, "%.3f");
                ImGui::DragFloat("Gravity Scale", &rb.GravityScale, 0.1f, -10.0f, 10.0f, "%.2f");
                ImGui::Checkbox("Fixed Rotation", &rb.FixedRotation);

                DrawVec3Control("Linear Vel", rb.LinearVelocity);
                DrawVec3Control("Angular Vel", rb.AngularVelocity);

                ImGui::TreePop();
            }
        }

        // ---- BOX COLLIDER COMPONENT ----
        if (entity.HasComponent<BoxColliderComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(BoxColliderComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Box Collider")) {

                auto& bc = entity.GetComponent<BoxColliderComponent>();

                DrawVec3Control("Half Extents", bc.HalfExtents, 0.5f);
                DrawVec3Control("Offset", bc.Offset);

                ImGui::DragFloat("Density", &bc.Density, 0.01f, 0.001f, 100.0f, "%.3f");
                ImGui::DragFloat("Friction", &bc.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
                ImGui::DragFloat("Restitution", &bc.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");

                ImGui::TreePop();
            }
        }

        // ---- SPHERE COLLIDER COMPONENT ----
        if (entity.HasComponent<SphereColliderComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(SphereColliderComponent).hash_code(),
                    ImGuiTreeNodeFlags_DefaultOpen, "Sphere Collider")) {

                auto& sc = entity.GetComponent<SphereColliderComponent>();

                ImGui::DragFloat("Radius", &sc.Radius, 0.01f, 0.001f, 100.0f, "%.3f");
                DrawVec3Control("Offset", sc.Offset);

                ImGui::DragFloat("Density", &sc.Density, 0.01f, 0.001f, 100.0f, "%.3f");
                ImGui::DragFloat("Friction", &sc.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
                ImGui::DragFloat("Restitution", &sc.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");

                ImGui::TreePop();
            }
        }

        ImGui::Separator();

        // Ortalamaya çalışmak için ufak bir boşluk
        ImGui::Spacing();
        ImGui::Spacing();

        float windowWidth = ImGui::GetWindowWidth();
        float buttonWidth = 150.0f;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0.0f))) {
            ImGui::OpenPopup("AddComponent");
        }

        if (ImGui::BeginPopup("AddComponent")) {
            if (!entity.HasComponent<CameraComponent>() && ImGui::MenuItem("Camera")) {
                entity.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<MeshRendererComponent>() && ImGui::MenuItem("Mesh Renderer")) {
                entity.AddComponent<MeshRendererComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<ScriptComponent>() && ImGui::MenuItem("Script (C#)")) {
                entity.AddComponent<ScriptComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<NativeScriptComponent>() && ImGui::MenuItem("Native Script (C++)")) {
                entity.AddComponent<NativeScriptComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<RigidbodyComponent>() && ImGui::MenuItem("Rigidbody")) {
                entity.AddComponent<RigidbodyComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<BoxColliderComponent>() && ImGui::MenuItem("Box Collider")) {
                entity.AddComponent<BoxColliderComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (!entity.HasComponent<SphereColliderComponent>() && ImGui::MenuItem("Sphere Collider")) {
                entity.AddComponent<SphereColliderComponent>();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

    }

}
