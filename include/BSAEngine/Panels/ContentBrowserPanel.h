#pragma once

#include <filesystem>

namespace BSA {

    class ContentBrowserPanel {
    public:
        ContentBrowserPanel();

        void OnImGuiRender();

    private:
        std::filesystem::path m_BaseDirectory;
        std::filesystem::path m_CurrentDirectory;

        bool m_ShowRenameModal = false;
        std::filesystem::path m_RenamingPath;
        char m_RenameBuffer[256] = "";
    };

}
