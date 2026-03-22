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
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Klavye destegi
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Docking (Pencereleri surukleyip yapistirma) destegi
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Pencereleri ana ekranin disina cikarabilme destegi

        // ImGui temasini koyu yap
        ImGui::StyleColorsDark();

        // Pencere kenarliklarini yumusat
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Backend entegrasyonlarini baslat
        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410"); // OpenGL 4.1 ve uzeri hedefleniyor
        
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