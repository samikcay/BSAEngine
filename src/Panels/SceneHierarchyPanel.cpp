#include "BSAEngine/Panels/SceneHierarchyPanel.h"
#include "BSAEngine/Scene/Components.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Scene/PrefabSerializer.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <cstring>
#include <filesystem>
#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace BSA {

    // Hierarchy search filter
    static char s_HierarchySearchBuffer[256] = "";

    // Helper: Get icon string for entity based on its components
    static const char* GetEntityIcon(Entity entity) {
        if (entity.HasComponent<CameraComponent>())            return "[Cam]";
        if (entity.HasComponent<DirectionalLightComponent>())  return "[Dir]";
        if (entity.HasComponent<PointLightComponent>())        return "[Pt]";
        if (entity.HasComponent<MeshRendererComponent>())      return "[Mesh]";
        if (entity.HasComponent<ScriptComponent>())            return "[CS]";
        if (entity.HasComponent<NativeScriptComponent>())      return "[CPP]";
        return "[Obj]";
    }

    // Helper: case-insensitive substring search
    static bool ContainsInsensitive(const std::string& str, const std::string& query) {
        if (query.empty()) return true;
        std::string lowerStr = str;
        std::string lowerQuery = query;
        for (auto& c : lowerStr) c = (char)std::tolower(c);
        for (auto& c : lowerQuery) c = (char)std::tolower(c);
        return lowerStr.find(lowerQuery) != std::string::npos;
    }

    SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context) {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context) {
        m_Context = context;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() {
        // ==================== HIERARCHY PANEL ====================
        ImGui::Begin("Hierarchy");

        if (m_Context) {
            // Search bar at top (Unity style)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.14f, 0.14f, 0.14f, 1.0f });
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextWithHint("##HierarchySearch", "Search...", s_HierarchySearchBuffer, sizeof(s_HierarchySearchBuffer));
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            std::string searchQuery = s_HierarchySearchBuffer;

            auto view = m_Context->Reg().view<RelationshipComponent>();
            for (auto entityID : view) {
                auto& rel = view.get<RelationshipComponent>(entityID);
                if (rel.Parent == entt::null) {
                    Entity entity{ entityID, m_Context.get() };
                    // When searching, show flat list of matching entities
                    if (!searchQuery.empty()) {
                        auto& tag = entity.GetComponent<TagComponent>().Tag;
                        if (ContainsInsensitive(tag, searchQuery)) {
                            DrawEntityNode(entity);
                        }
                        // Also search children
                        if (entity.HasComponent<RelationshipComponent>()) {
                            auto& childRel = entity.GetComponent<RelationshipComponent>();
                            for (auto childHandle : childRel.Children) {
                                Entity child{ childHandle, m_Context.get() };
                                auto& childTag = child.GetComponent<TagComponent>().Tag;
                                if (ContainsInsensitive(childTag, searchQuery)) {
                                    DrawEntityNode(child);
                                }
                            }
                        }
                    } else {
                        DrawEntityNode(entity);
                    }
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_SelectionContext = {};

            if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                if (ImGui::MenuItem("Create Empty"))
                    m_Context->CreateEntity("GameObject");

                ImGui::EndPopup();
            }
        }

        ImGui::End();

        // ==================== INSPECTOR PANEL ====================
        ImGui::Begin("Inspector");
        if (m_SelectionContext) {
            DrawComponents(m_SelectionContext);
        } else {
            // Empty state message
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 textSize = ImGui::CalcTextSize("Select a GameObject to inspect");
            ImGui::SetCursorPos(ImVec2((avail.x - textSize.x) * 0.5f, avail.y * 0.4f));
            ImGui::TextDisabled("Select a GameObject to inspect");
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        const char* icon = GetEntityIcon(entity);

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

        // Check if entity has children
        bool hasChildren = false;
        if (entity.HasComponent<RelationshipComponent>()) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            hasChildren = !rel.Children.empty();
        }
        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        // Unity-style selection highlight
        if (m_SelectionContext == entity) {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.20f, 0.43f, 0.66f, 1.0f });
        }

        // Format: [Icon] EntityName
        char label[512];
        snprintf(label, sizeof(label), "%s %s", icon, tag.c_str());

        bool opened = ImGui::TreeNodeEx((void*)(uintptr_t)((uint32_t)entity + 1), flags, "%s", label);

        if (m_SelectionContext == entity)
            ImGui::PopStyleColor(2);

        if (ImGui::IsItemClicked()) {
            m_SelectionContext = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem("EntityNodeContext")) {
            if (ImGui::MenuItem("Rename")) {}
            if (ImGui::MenuItem("Duplicate")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Create Prefab")) {
                std::string filepath = "assets/" + entity.GetComponent<TagComponent>().Tag + ".bsaprefab";
                PrefabSerializer::Serialize(filepath, entity);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened && hasChildren) {
            auto& rel = entity.GetComponent<RelationshipComponent>();
            for (auto childHandle : rel.Children) {
                Entity childEntity{ childHandle, m_Context.get() };
                DrawEntityNode(childEntity);
            }
            ImGui::TreePop();
        }

        if (entityDeleted) {
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
            m_Context->DestroyEntity(entity);
        }
    }

    // Unity-style Vec3 control with colored X/Y/Z labels
    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* boldFont = io.Fonts->Fonts.Size > 1 ? io.Fonts->Fonts[1] : nullptr;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        // X - Red accent bar
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.67f, 0.20f, 0.20f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.80f, 0.25f, 0.25f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.55f, 0.15f, 0.15f, 1.0f });
        if (boldFont) ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        if (boldFont) ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Y - Green accent bar
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.27f, 0.55f, 0.20f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.32f, 0.65f, 0.25f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.22f, 0.45f, 0.15f, 1.0f });
        if (boldFont) ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        if (boldFont) ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Z - Blue accent bar
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.20f, 0.33f, 0.67f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.25f, 0.40f, 0.80f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.15f, 0.28f, 0.55f, 1.0f });
        if (boldFont) ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        if (boldFont) ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }

    // Unity-style component header with dark bar, arrow, and settings gear
    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
            | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_FramePadding;

        if (entity.HasComponent<T>()) {
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            // Unity-style dark component header
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.18f, 0.18f, 0.18f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.23f, 0.23f, 0.23f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f });

            float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)(uintptr_t)(typeid(T).hash_code() + 1), treeNodeFlags, "%s", name.c_str());
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();

            // Settings gear button (right-aligned)
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
            if (ImGui::Button(":", ImVec2{ lineHeight, lineHeight })) {
                ImGui::OpenPopup("ComponentSettings");
            }
            ImGui::PopStyleColor();

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")) {
                if (ImGui::MenuItem("Reset")) {}
                if (ImGui::MenuItem("Remove Component"))
                    removeComponent = true;
                if (ImGui::MenuItem("Copy Component")) {}
                if (ImGui::MenuItem("Paste Component Values")) {}

                ImGui::EndPopup();
            }

            if (open) {
                ImGui::Spacing();
                uiFunction(component);
                ImGui::Spacing();
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    // Helper: Unity-style property row
    static void BeginPropertyRow(const char* label, float labelWidth = 120.0f) {
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, labelWidth);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", label);
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }

    static void EndPropertyRow() {
        ImGui::PopItemWidth();
        ImGui::Columns(1);
    }


    void SceneHierarchyPanel::DrawComponents(Entity entity) {
        // Entity active toggle + tag name (Unity style)
        if (entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            // Entity icon
            ImGui::TextDisabled("%s", GetEntityIcon(entity));
            ImGui::SameLine();

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), _TRUNCATE);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 130);
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Add Component button (Unity style - full width at bottom area)
        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent")) {
            // Search in popup
            static char componentSearch[128] = "";
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextWithHint("##CompSearch", "Search...", componentSearch, sizeof(componentSearch));
            ImGui::Separator();

            std::string query = componentSearch;

            if (ContainsInsensitive("Camera", query) && ImGui::MenuItem("Camera")) {
                m_SelectionContext.AddComponent<CameraComponent>();
                componentSearch[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
            if (ContainsInsensitive("Mesh Renderer", query) && ImGui::MenuItem("Mesh Renderer")) {
                m_SelectionContext.AddComponent<MeshRendererComponent>();
                componentSearch[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::BeginMenu("Rendering")) {
                if (ContainsInsensitive("Directional Light", query) && ImGui::MenuItem("Directional Light")) {
                    m_SelectionContext.AddComponent<DirectionalLightComponent>();
                    componentSearch[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                if (ContainsInsensitive("Point Light", query) && ImGui::MenuItem("Point Light")) {
                    m_SelectionContext.AddComponent<PointLightComponent>();
                    componentSearch[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics 2D")) {
                if (ContainsInsensitive("Rigidbody 2D", query) && ImGui::MenuItem("Rigidbody 2D")) {
                    m_SelectionContext.AddComponent<Rigidbody2DComponent>();
                    componentSearch[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                if (ContainsInsensitive("Box Collider 2D", query) && ImGui::MenuItem("Box Collider 2D")) {
                    m_SelectionContext.AddComponent<BoxCollider2DComponent>();
                    componentSearch[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                if (ContainsInsensitive("Circle Collider 2D", query) && ImGui::MenuItem("Circle Collider 2D")) {
                    m_SelectionContext.AddComponent<CircleCollider2DComponent>();
                    componentSearch[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }

            if (ContainsInsensitive("Script", query) && ImGui::MenuItem("Script (C#)")) {
                m_SelectionContext.AddComponent<ScriptComponent>();
                componentSearch[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Separator();

        // ---- Components ----

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
            glm::vec3 translation = { component.Translation.x, component.Translation.y, component.Translation.z };
            DrawVec3Control("Position", translation);
            component.Translation = { translation.x, translation.y, translation.z };

            glm::vec3 rotation = { glm::degrees(component.Rotation.x), glm::degrees(component.Rotation.y), glm::degrees(component.Rotation.z) };
            DrawVec3Control("Rotation", rotation);
            component.Rotation = { glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) };

            glm::vec3 scale = { component.Scale.x, component.Scale.y, component.Scale.z };
            DrawVec3Control("Scale", scale, 1.0f);
            component.Scale = { scale.x, scale.y, scale.z };
        });

        DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
            auto& camera = component.Camera;

            BeginPropertyRow("Primary");
            ImGui::Checkbox("##Primary", &component.Primary);
            EndPropertyRow();

            float fov = glm::degrees(camera.GetFOV());
            BeginPropertyRow("Field of View");
            if (ImGui::SliderFloat("##FOV", &fov, 1.0f, 179.0f, "%.1f"))
                camera.SetFOV(glm::radians(fov));
            EndPropertyRow();

            float nearClip = camera.GetNear();
            BeginPropertyRow("Clipping Near");
            if (ImGui::DragFloat("##Near", &nearClip, 0.01f, 0.001f, 0.0f, "%.3f"))
                camera.SetNear(nearClip);
            EndPropertyRow();

            float farClip = camera.GetFar();
            BeginPropertyRow("Clipping Far");
            if (ImGui::DragFloat("##Far", &farClip, 1.0f, 0.0f, 0.0f, "%.1f"))
                camera.SetFar(farClip);
            EndPropertyRow();
        });

        DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component) {
            BeginPropertyRow("Mesh");
            ImGui::TextDisabled("%s", component.Mesh ? "Cube (Static)" : "None");
            EndPropertyRow();

            if (component.Mat) {
                BeginPropertyRow("Material");
                ImGui::TextDisabled("%s", component.Mat->GetName().c_str());
                EndPropertyRow();

                // Material properties
                for (auto& [name, value] : component.Mat->GetFloats()) {
                    BeginPropertyRow(name.c_str());
                    ImGui::DragFloat((std::string("##") + name).c_str(), &value, 0.01f);
                    EndPropertyRow();
                }
                for (auto& [name, value] : component.Mat->GetVec3s()) {
                    BeginPropertyRow(name.c_str());
                    ImGui::ColorEdit3((std::string("##") + name).c_str(), glm::value_ptr(value));
                    EndPropertyRow();
                }
                for (auto& [name, value] : component.Mat->GetVec4s()) {
                    BeginPropertyRow(name.c_str());
                    ImGui::ColorEdit4((std::string("##") + name).c_str(), glm::value_ptr(value));
                    EndPropertyRow();
                }
            }
        });

        DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component) {
            BeginPropertyRow("Color");
            ImGui::ColorEdit3("##Color", glm::value_ptr(component.Color));
            EndPropertyRow();

            BeginPropertyRow("Intensity");
            ImGui::SliderFloat("##Intensity", &component.Intensity, 0.0f, 10.0f, "%.2f");
            EndPropertyRow();
        });

        DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component) {
            BeginPropertyRow("Color");
            ImGui::ColorEdit3("##Color", glm::value_ptr(component.Color));
            EndPropertyRow();

            BeginPropertyRow("Intensity");
            ImGui::SliderFloat("##Intensity", &component.Intensity, 0.0f, 10.0f, "%.2f");
            EndPropertyRow();

            BeginPropertyRow("Range");
            ImGui::DragFloat("##Constant", &component.Constant, 0.01f);
            EndPropertyRow();

            BeginPropertyRow("Linear");
            ImGui::DragFloat("##Linear", &component.Linear, 0.001f);
            EndPropertyRow();

            BeginPropertyRow("Quadratic");
            ImGui::DragFloat("##Quadratic", &component.Quadratic, 0.0001f, 0.0f, 0.0f, "%.4f");
            EndPropertyRow();
        });

        DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
            const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
            const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

            BeginPropertyRow("Body Type");
            if (ImGui::BeginCombo("##Body Type", currentBodyTypeString)) {
                for (int i = 0; i < 3; i++) {
                    bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.Type = (BodyType)i;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            EndPropertyRow();

            BeginPropertyRow("Fixed Rotation");
            ImGui::Checkbox("##Fixed Rotation", &component.FixedRotation);
            EndPropertyRow();
        });

        DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component) {
            BeginPropertyRow("Offset");
            glm::vec2 offset = { component.Offset.x, component.Offset.y };
            if (ImGui::DragFloat2("##Offset", glm::value_ptr(offset), 0.01f))
                component.Offset = { offset.x, offset.y, 0.0f };
            EndPropertyRow();

            BeginPropertyRow("Size");
            glm::vec2 size = { component.HalfExtents.x, component.HalfExtents.y };
            if (ImGui::DragFloat2("##Size", glm::value_ptr(size), 0.01f))
                component.HalfExtents = { size.x, size.y, 0.5f };
            EndPropertyRow();

            BeginPropertyRow("Friction");
            ImGui::SliderFloat("##Friction", &component.Friction, 0.0f, 1.0f, "%.2f");
            EndPropertyRow();

            BeginPropertyRow("Restitution");
            ImGui::SliderFloat("##Restitution", &component.Restitution, 0.0f, 1.0f, "%.2f");
            EndPropertyRow();
        });

        DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) {
            BeginPropertyRow("Offset");
            glm::vec2 offset = { component.Offset.x, component.Offset.y };
            if (ImGui::DragFloat2("##Offset", glm::value_ptr(offset), 0.01f))
                component.Offset = { offset.x, offset.y, 0.0f };
            EndPropertyRow();

            BeginPropertyRow("Radius");
            ImGui::DragFloat("##Radius", &component.Radius, 0.01f);
            EndPropertyRow();

            BeginPropertyRow("Friction");
            ImGui::SliderFloat("##Friction", &component.Friction, 0.0f, 1.0f, "%.2f");
            EndPropertyRow();

            BeginPropertyRow("Restitution");
            ImGui::SliderFloat("##Restitution", &component.Restitution, 0.0f, 1.0f, "%.2f");
            EndPropertyRow();
        });

        DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context.get()](auto& component) mutable {
            BeginPropertyRow("Class");

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), component.ClassName.c_str(), _TRUNCATE);

            if (ImGui::InputText("##Class", buffer, sizeof(buffer)))
                component.ClassName = buffer;

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                    const char* path = (const char*)payload->Data;
                    std::filesystem::path filepath = path;
                    if (filepath.extension() == ".cs") {
                        component.ClassName = "Sandbox." + filepath.stem().string();
                    }
                }
                ImGui::EndDragDropTarget();
            }
            EndPropertyRow();
        });

        // "Add Component" button at bottom (Unity-style centered)
        ImGui::Spacing();
        ImGui::Spacing();
        float buttonWidth = 230.0f;
        float windowWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f + ImGui::GetCursorPosX());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.27f, 0.27f, 0.27f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.32f, 0.32f, 0.32f, 1.0f });
        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 28))) {
            ImGui::OpenPopup("AddComponent");
        }
        ImGui::PopStyleColor(2);
    }


}
