#include "BSAEngine/Core/Application.h"
#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Events/ApplicationEvent.h"
#include <GLFW/glfw3.h> // Sadece GLFWWindow native window cast için gecici kalabilir (veya silinebilir)

namespace BSA {

    Application* Application::s_Instance = nullptr;

    Application::Application() {
        if (s_Instance) {
            BSA_ENGINE_ERROR("Application zaten olusturulmus!");
            return;
        }
        s_Instance = this;

        // Varsayılan pencere yaratılıyor
        m_Window = std::unique_ptr<Window>(Window::Create());
        
        // Window'dan gelen eventleri (OnEvent) fonksiyonuna bagla
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    }

    Application::~Application() {
        BSA_ENGINE_INFO("Application kapatiliyor.");
    }

    void Application::Run() {
        BSA_ENGINE_INFO("Oyun Dongusu (Game Loop) Basladi.");

        // Ana Oyun Döngüsü
        while (m_Running) {
            
            // İleride buraya:
            // 1. Delta Time Hesaplaması eklenecek.
            // 2. Layer (Katmalar) güncellemeleri yapılacak: for(Layer* layer : m_LayerStack) layer->OnUpdate(dt);
            // 3. ImGui (Arayüz) render döngüsü çalıştırılacak.
            
            // Pencere güncellemeleri (Swap Buffers, Poll Events)
            m_Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

        // Test amaçlı tüm Eventleri bas (İlerde kaldırılacak)
        // BSA_ENGINE_TRACE("{0}", e.ToString());
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

} // namespace BSA
