#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <string>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BSAEngine/ImGui/ImGuiLayer.h"
#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Log/Log.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>

namespace BSA {

    ImGuiLayer::ImGuiLayer() {
    }

    ImGuiLayer::~ImGuiLayer() {
    }

    void ImGuiLayer::Init() {
        // ImGui context'ini yarat
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Disabled: Windows stay within main window

        // LOAD FONTS
        float fontSize = 16.0f;
        std::string regularPath = "assets/fonts/open sans/static/OpenSans-Regular.ttf";
        std::string boldPath = "assets/fonts/open sans/static/OpenSans-Bold.ttf";

        if (std::filesystem::exists(regularPath)) {
            io.FontDefault = io.Fonts->AddFontFromFileTTF(regularPath.c_str(), fontSize);
        }

        if (std::filesystem::exists(boldPath)) {
            io.Fonts->AddFontFromFileTTF(boldPath.c_str(), fontSize);
            io.Fonts->AddFontFromFileTTF(boldPath.c_str(), fontSize * 1.1f);
        }

        // UNITY-STYLE DARK THEME
        auto& colors = ImGui::GetStyle().Colors;

        // Main backgrounds - Unity's warm dark grays
        colors[ImGuiCol_WindowBg]             = ImVec4{ 0.22f, 0.22f, 0.22f, 1.0f }; // #383838
        colors[ImGuiCol_ChildBg]              = ImVec4{ 0.22f, 0.22f, 0.22f, 1.0f };
        colors[ImGuiCol_PopupBg]              = ImVec4{ 0.24f, 0.24f, 0.24f, 0.98f };
        colors[ImGuiCol_Border]               = ImVec4{ 0.11f, 0.11f, 0.11f, 1.0f };
        colors[ImGuiCol_BorderShadow]         = ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };

        // Text
        colors[ImGuiCol_Text]                 = ImVec4{ 0.86f, 0.86f, 0.86f, 1.0f }; // #DCDCDC
        colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.50f, 0.50f, 0.50f, 1.0f };
        colors[ImGuiCol_TextSelectedBg]       = ImVec4{ 0.17f, 0.40f, 0.63f, 0.60f };

        // Headers - Unity uses subtle dark headers
        colors[ImGuiCol_Header]               = ImVec4{ 0.27f, 0.27f, 0.27f, 1.0f }; // #454545
        colors[ImGuiCol_HeaderHovered]         = ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f };
        colors[ImGuiCol_HeaderActive]          = ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f }; // Selection blue

        // Buttons
        colors[ImGuiCol_Button]               = ImVec4{ 0.30f, 0.30f, 0.30f, 1.0f }; // #4D4D4D
        colors[ImGuiCol_ButtonHovered]         = ImVec4{ 0.35f, 0.35f, 0.35f, 1.0f };
        colors[ImGuiCol_ButtonActive]          = ImVec4{ 0.25f, 0.25f, 0.25f, 1.0f };

        // Frame BG - Input fields, checkboxes
        colors[ImGuiCol_FrameBg]              = ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f }; // #2A2A2A
        colors[ImGuiCol_FrameBgHovered]        = ImVec4{ 0.19f, 0.19f, 0.19f, 1.0f };
        colors[ImGuiCol_FrameBgActive]         = ImVec4{ 0.14f, 0.14f, 0.14f, 1.0f };

        // Tabs - Unity style: active tab matches window bg, inactive is darker
        colors[ImGuiCol_Tab]                  = ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f }; // Inactive
        colors[ImGuiCol_TabHovered]            = ImVec4{ 0.27f, 0.27f, 0.27f, 1.0f };
        colors[ImGuiCol_TabActive]             = ImVec4{ 0.22f, 0.22f, 0.22f, 1.0f }; // Matches window bg
        colors[ImGuiCol_TabUnfocused]          = ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive]    = ImVec4{ 0.20f, 0.20f, 0.20f, 1.0f };

        // Title bar
        colors[ImGuiCol_TitleBg]              = ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f }; // #2A2A2A
        colors[ImGuiCol_TitleBgActive]         = ImVec4{ 0.16f, 0.16f, 0.16f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4{ 0.14f, 0.14f, 0.14f, 1.0f };

        // Menu bar
        colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.22f, 0.22f, 0.22f, 1.0f };

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg]          = ImVec4{ 0.16f, 0.16f, 0.16f, 0.5f };
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4{ 0.35f, 0.35f, 0.35f, 1.0f };
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4{ 0.40f, 0.40f, 0.40f, 1.0f };
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4{ 0.45f, 0.45f, 0.45f, 1.0f };

        // Selection / Accent - Unity's blue selection
        colors[ImGuiCol_CheckMark]            = ImVec4{ 0.86f, 0.86f, 0.86f, 1.0f };
        colors[ImGuiCol_SliderGrab]            = ImVec4{ 0.44f, 0.44f, 0.44f, 1.0f };
        colors[ImGuiCol_SliderGrabActive]      = ImVec4{ 0.50f, 0.50f, 0.50f, 1.0f };

        // Separators
        colors[ImGuiCol_Separator]            = ImVec4{ 0.11f, 0.11f, 0.11f, 1.0f };
        colors[ImGuiCol_SeparatorHovered]      = ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f };
        colors[ImGuiCol_SeparatorActive]       = ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f };

        // Resize grip
        colors[ImGuiCol_ResizeGrip]           = ImVec4{ 0.30f, 0.30f, 0.30f, 0.25f };
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4{ 0.17f, 0.40f, 0.63f, 0.67f };
        colors[ImGuiCol_ResizeGripActive]      = ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f };

        // Docking
        colors[ImGuiCol_DockingPreview]       = ImVec4{ 0.17f, 0.40f, 0.63f, 0.70f };
        colors[ImGuiCol_DockingEmptyBg]        = ImVec4{ 0.19f, 0.19f, 0.19f, 1.0f };

        // Nav highlight
        colors[ImGuiCol_NavHighlight]         = ImVec4{ 0.17f, 0.40f, 0.63f, 1.0f };

        // Style settings - Unity uses minimal rounding, compact spacing
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding     = ImVec2(4, 4);
        style.FramePadding      = ImVec2(4, 2);
        style.CellPadding       = ImVec2(4, 2);
        style.ItemSpacing       = ImVec2(4, 4);
        style.ItemInnerSpacing  = ImVec2(4, 4);
        style.IndentSpacing     = 14.0f;
        style.ScrollbarSize     = 14.0f;
        style.GrabMinSize       = 10.0f;
        style.WindowBorderSize  = 1.0f;
        style.ChildBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;
        style.FrameBorderSize   = 0.0f;
        style.TabBorderSize     = 0.0f;
        style.WindowRounding    = 0.0f;  // Unity uses no rounding
        style.ChildRounding     = 0.0f;
        style.FrameRounding     = 2.0f;  // Slight rounding on inputs only
        style.PopupRounding     = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding      = 2.0f;
        style.TabRounding       = 2.0f;
        style.WindowTitleAlign  = ImVec2(0.0f, 0.5f); // Left-aligned titles like Unity


        // Backend entegrasyonlarini baslat
        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410"); 
        
        BSA_ENGINE_INFO("ImGuiLayer basariyla baslatildi.");
    }

    void ImGuiLayer::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin() {
        // Yeni frame icin arka planda gerekli verileri hazirla
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End() {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        // ImGui cizimlerini gerceklestir
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Eger viewport aktifse, isletim sistemi seviyesinde ek pencereleri yonet
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::OnEvent(Event& e) {
        // ImGui acikken (Mouse UI ustundeyken), event'lerin oyun dunyasina gecmesini engelle
        if (m_BlockEvents) {
            ImGuiIO& io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

}