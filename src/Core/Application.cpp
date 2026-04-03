#include "BSAEngine/Core/Application.h"
#include "BSAEngine/ImGui/ImGuiLayer.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Events/ApplicationEvent.h"
#include "BSAEngine/Renderer/Renderer.h"
#include "BSAEngine/Asset/AssetManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BSA {

    Application* Application::s_Instance = nullptr;

    Application::Application() {
        if (s_Instance) {
            BSA_ENGINE_ERROR("Application zaten olusturulmus!");
            return;
        }
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        Renderer::Init();
        AssetManager::Init();

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init();
    }

    Application::~Application() {
        m_ImGuiLayer->Shutdown();
        AssetManager::Shutdown();
        Renderer::Shutdown();
    }

    void Application::PushLayer(Layer* layer) {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay) {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::Run() {
        float lastFrameTime = 0.0f;

        while (m_Running) {
            float time = (float)glfwGetTime();
            float timestep = time - lastFrameTime;
            lastFrameTime = time;

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(timestep);

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }

        if (!e.Handled)
            m_ImGuiLayer->OnEvent(e);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

} // namespace BSA
