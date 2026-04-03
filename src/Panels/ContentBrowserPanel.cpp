#include "BSAEngine/Panels/ContentBrowserPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

namespace BSA {

    // File type color helpers
    static ImU32 GetFileTypeColor(const std::string& ext) {
        if (ext == ".cs")         return IM_COL32(86, 156, 214, 255);   // C# blue
        if (ext == ".bsaprefab")  return IM_COL32(100, 180, 230, 255);  // Prefab blue
        if (ext == ".bsascene")   return IM_COL32(120, 200, 130, 255);  // Scene green
        if (ext == ".glsl" || ext == ".vert" || ext == ".frag")
                                  return IM_COL32(200, 130, 200, 255);  // Shader purple
        if (ext == ".png" || ext == ".jpg" || ext == ".tga")
                                  return IM_COL32(200, 180, 100, 255);  // Texture yellow
        return IM_COL32(180, 180, 180, 255);                            // Default gray
    }

    static const char* GetFileTypeLabel(const std::string& ext) {
        if (ext == ".cs")         return "C#";
        if (ext == ".bsaprefab")  return "Prefab";
        if (ext == ".bsascene")   return "Scene";
        if (ext == ".glsl")       return "Shader";
        if (ext == ".png" || ext == ".jpg" || ext == ".tga") return "Tex";
        return "";
    }

    ContentBrowserPanel::ContentBrowserPanel()
        : m_BaseDirectory("assets"), m_CurrentDirectory("assets") {

        if (!std::filesystem::exists(m_BaseDirectory)) {
            std::filesystem::create_directory(m_BaseDirectory);
        }
    }

    void ContentBrowserPanel::OnImGuiRender() {
        ImGui::Begin("Project");

        // ---- Top bar: breadcrumb + search ----
        {
            // Back button
            bool canGoBack = m_CurrentDirectory != m_BaseDirectory;
            if (!canGoBack) ImGui::BeginDisabled();
            if (ImGui::Button("<")) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
            if (!canGoBack) ImGui::EndDisabled();

            ImGui::SameLine();

            // Breadcrumb path
            auto relativePath = std::filesystem::relative(m_CurrentDirectory, m_BaseDirectory);
            std::string relativePathString = relativePath.string();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.30f, 0.30f, 0.30f, 0.5f });

            if (ImGui::Button("Assets")) {
                m_CurrentDirectory = m_BaseDirectory;
            }

            if (relativePathString != ".") {
                std::filesystem::path currentPathBuild = "";
                for (auto& part : relativePath) {
                    ImGui::SameLine();
                    ImGui::TextDisabled(">");
                    ImGui::SameLine();

                    if (ImGui::Button(part.string().c_str())) {
                        m_CurrentDirectory = m_BaseDirectory / currentPathBuild / part;
                    }
                    currentPathBuild /= part;
                }
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();

            // Search on the right
            ImGui::SameLine(ImGui::GetWindowWidth() - 220);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 3));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.14f, 0.14f, 0.14f, 1.0f });
            static char searchBuf[128] = "";
            ImGui::SetNextItemWidth(200);
            ImGui::InputTextWithHint("##ProjectSearch", "Search...", searchBuf, sizeof(searchBuf));
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            std::string searchQuery = searchBuf;

            ImGui::Separator();
            ImGui::Spacing();

            // ---- Grid view ----
            static float thumbnailSize = 80.0f;
            static float padding = 12.0f;
            float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columnCount = (int)(panelWidth / cellSize);
            if (columnCount < 1)
                columnCount = 1;

            ImGui::Columns(columnCount, 0, false);

            for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
                const auto& path = directoryEntry.path();
                auto relPath = std::filesystem::relative(path, m_BaseDirectory);
                std::string filenameString = path.filename().string();

                // Apply search filter
                if (!searchQuery.empty()) {
                    std::string lowerName = filenameString;
                    std::string lowerQuery = searchQuery;
                    for (auto& c : lowerName) c = (char)std::tolower(c);
                    for (auto& c : lowerQuery) c = (char)std::tolower(c);
                    if (lowerName.find(lowerQuery) == std::string::npos)
                        continue;
                }

                ImGui::PushID(filenameString.c_str());

                // Thumbnail button
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.22f, 0.22f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.18f, 0.18f, 0.6f));

                if (directoryEntry.is_directory()) {
                    ImGui::Button("##folder", { thumbnailSize, thumbnailSize });

                    // Unity-style folder icon
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 pMin = ImGui::GetItemRectMin();
                    ImVec2 pMax = ImGui::GetItemRectMax();
                    float m = 14.0f; // margin

                    // Folder body
                    float bodyTop = pMin.y + m + 12;
                    drawList->AddRectFilled(
                        { pMin.x + m, bodyTop },
                        { pMax.x - m, pMax.y - m },
                        IM_COL32(200, 165, 55, 255), 3.0f);

                    // Folder tab
                    drawList->AddRectFilled(
                        { pMin.x + m, pMin.y + m + 4 },
                        { pMin.x + m + 28, bodyTop + 2 },
                        IM_COL32(180, 145, 40, 255), 2.0f);

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        m_CurrentDirectory /= path.filename();
                    }
                } else {
                    ImGui::Button("##file", { thumbnailSize, thumbnailSize });

                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 pMin = ImGui::GetItemRectMin();
                    ImVec2 pMax = ImGui::GetItemRectMax();
                    float m = 16.0f;
                    std::string ext = path.extension().string();

                    // File body with color-coded type
                    ImU32 fileColor = GetFileTypeColor(ext);
                    drawList->AddRectFilled(
                        { pMin.x + m, pMin.y + m },
                        { pMax.x - m, pMax.y - m },
                        IM_COL32(45, 45, 45, 255), 2.0f);
                    // Color accent bar at top
                    drawList->AddRectFilled(
                        { pMin.x + m, pMin.y + m },
                        { pMax.x - m, pMin.y + m + 4 },
                        fileColor, 2.0f);
                    // File type label
                    const char* typeLabel = GetFileTypeLabel(ext);
                    if (typeLabel[0]) {
                        ImVec2 labelSize = ImGui::CalcTextSize(typeLabel);
                        float cx = (pMin.x + m + pMax.x - m - labelSize.x) * 0.5f;
                        float cy = (pMin.y + m + pMax.y - m - labelSize.y) * 0.5f;
                        drawList->AddText({ cx, cy }, fileColor, typeLabel);
                    }
                    // Outline
                    drawList->AddRect(
                        { pMin.x + m, pMin.y + m },
                        { pMax.x - m, pMax.y - m },
                        IM_COL32(60, 60, 60, 255), 2.0f);

                    if (ImGui::BeginDragDropSource()) {
                        std::string itemPathStr = relPath.string();
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathStr.c_str(), itemPathStr.size() + 1);
                        ImGui::Text("%s", filenameString.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
                ImGui::PopStyleColor(3);

                // Filename below icon - centered, truncated
                float textWidth = ImGui::CalcTextSize(filenameString.c_str()).x;
                if (textWidth > thumbnailSize) {
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
                    ImGui::TextWrapped("%s", filenameString.c_str());
                    ImGui::PopTextWrapPos();
                } else {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (thumbnailSize - textWidth) * 0.5f);
                    ImGui::Text("%s", filenameString.c_str());
                }

                // Context menu per item
                if (ImGui::BeginPopupContextItem("ContentItemContext")) {
                    if (ImGui::MenuItem("Rename")) {
                        m_ShowRenameModal = true;
                        m_RenamingPath = path;
                        strncpy_s(m_RenameBuffer, sizeof(m_RenameBuffer), filenameString.c_str(), _TRUNCATE);
                    }
                    if (ImGui::MenuItem("Show in Explorer")) {}
                    ImGui::Separator();
                    if (ImGui::MenuItem("Delete")) {
                        std::filesystem::remove_all(path);
                    }
                    ImGui::EndPopup();
                }

                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::Columns(1);
        }

        // Right-click context menu on empty space
        if (ImGui::BeginPopupContextWindow("ContentBrowserContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::BeginMenu("Create")) {
                if (ImGui::MenuItem("Folder")) {
                    int i = 0;
                    std::filesystem::path newFolderPath;
                    do {
                        std::string folderName = "New Folder" + (i > 0 ? " (" + std::to_string(i) + ")" : "");
                        newFolderPath = m_CurrentDirectory / folderName;
                        i++;
                    } while (std::filesystem::exists(newFolderPath));
                    std::filesystem::create_directory(newFolderPath);
                }

                if (ImGui::MenuItem("C# Script")) {
                    int i = 0;
                    std::filesystem::path newFilePath;
                    std::string className;
                    do {
                        className = "NewScript" + (i > 0 ? std::to_string(i) : "");
                        newFilePath = m_CurrentDirectory / (className + ".cs");
                        i++;
                    } while (std::filesystem::exists(newFilePath));

                    std::ofstream fout(newFilePath);
                    if (fout.is_open()) {
                        fout << "using System;\nusing BSAEngine;\n\nnamespace Sandbox\n{\n    public class " << className << " : Entity\n    {\n        protected override void OnCreate() {}\n        protected override void OnUpdate(float ts) {}\n    }\n}\n";
                        fout.close();
                    }
                }

                if (ImGui::MenuItem("Material")) {}
                if (ImGui::MenuItem("Shader")) {}

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Show in Explorer")) {}
            if (ImGui::MenuItem("Refresh")) {}
            ImGui::EndPopup();
        }

        // Rename modal
        if (m_ShowRenameModal) {
            ImGui::OpenPopup("Rename");
        }

        if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("New name:");
            ImGui::SetNextItemWidth(280);
            ImGui::InputText("##newname", m_RenameBuffer, sizeof(m_RenameBuffer));

            ImGui::Spacing();
            if (ImGui::Button("Rename", ImVec2(130, 0))) {
                std::filesystem::path newPath = m_RenamingPath.parent_path() / m_RenameBuffer;
                if (!std::filesystem::exists(newPath) && newPath != m_RenamingPath) {
                    std::filesystem::rename(m_RenamingPath, newPath);
                }
                m_ShowRenameModal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(130, 0))) {
                m_ShowRenameModal = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

}
