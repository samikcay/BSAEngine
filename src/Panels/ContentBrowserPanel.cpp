#include "BSAEngine/Panels/ContentBrowserPanel.h"
#include <imgui.h>
#include <fstream>
#include <string>

namespace BSA {

    ContentBrowserPanel::ContentBrowserPanel()
        : m_BaseDirectory("assets"), m_CurrentDirectory("assets") {
        
        if (!std::filesystem::exists(m_BaseDirectory)) {
            std::filesystem::create_directory(m_BaseDirectory);
        }
    }

    void ContentBrowserPanel::OnImGuiRender() {
        ImGui::Begin("Content Browser");

        if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory)) {
            if (ImGui::Button("<- Back")) {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, m_BaseDirectory);
            std::string filenameString = relativePath.filename().string();

            ImGui::PushID(filenameString.c_str());

            if (directoryEntry.is_directory()) {
                if (ImGui::Button(filenameString.c_str(), { thumbnailSize, thumbnailSize })) {
                    m_CurrentDirectory /= path.filename();
                }
            } else {
                ImGui::Button(filenameString.c_str(), { thumbnailSize, thumbnailSize });

                // ImGui Drag and Drop Source (Sadece dosyalar icin)
                if (ImGui::BeginDragDropSource()) {
                    std::string itemPathStr = relativePath.string();
                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathStr.c_str(), itemPathStr.size() + 1);
                    
                    ImGui::Text("%s", filenameString.c_str());
                    ImGui::EndDragDropSource();
                }
            }

            // Secili Oge Uzende Sag Tik (Silme)
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Sil (Delete)")) {
                    std::filesystem::remove_all(path);
                }
                ImGui::EndPopup();
            }

            ImGui::TextWrapped("%s", filenameString.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }
        ImGui::Columns(1);

        // Right-click context menu (bos alana veya popup)
        if (ImGui::BeginPopupContextWindow("ContentBrowserContext")) {
            if (ImGui::BeginMenu("Olustur (Create)")) {
                if (ImGui::MenuItem("Klasor (Folder)")) {
                    int i = 0;
                    std::filesystem::path newFolderPath;
                    do {
                        std::string folderName = "Yeni Klasor" + (i > 0 ? " (" + std::to_string(i) + ")" : "");
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
                        fout << "using System;\nusing BSAEngine;\n\nnamespace Sandbox\n{\n    public class " << className << " : Entity\n    {\n        protected override void OnCreate()\n        {\n        }\n\n        protected override void OnUpdate(float ts)\n        {\n        }\n    }\n}\n";
                        fout.close();
                    }
                }

                if (ImGui::MenuItem("Bos Dosya (Empty File)")) {
                    int i = 0;
                    std::filesystem::path newFilePath;
                    do {
                        std::string filename = "Yeni Dosya" + (i > 0 ? " (" + std::to_string(i) + ")" : "") + ".txt";
                        newFilePath = m_CurrentDirectory / filename;
                        i++;
                    } while (std::filesystem::exists(newFilePath));

                    std::ofstream fout(newFilePath);
                    fout.close();
                }

                ImGui::EndMenu();
            }
            
            ImGui::EndPopup();
        }

        ImGui::End(); // Content Browser
    }

}
