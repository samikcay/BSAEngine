#pragma once

#include "BSAEngine/Core/Window.h"
#include "BSAEngine/Core/LayerStack.h"
#include <memory>

namespace BSA {

    // ---------------------------------------------------------------
    // Motorun Ana Sınıfı (Application)
    // Game Loop'u (Oyun Döngüsü) yönetir, Pencere'ye (Window) sahiptir.
    // ---------------------------------------------------------------
    class Event;
    class WindowCloseEvent;
    class ImGuiLayer;

    class Application {
    public:
        Application();
        virtual ~Application();

        // Motorun ana döngüsünü başlatır
        void Run();

        // Pencereden veya başka yerlerden gelen olayları işler
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        // Motoru güvenli şekilde kapatmak için
        void Close() { m_Running = false; }

        inline Window& GetWindow() { return *m_Window; }
        
        static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;

        static Application* s_Instance;
    };

    // Client (Oyun) tarafı tarafından overload edilecek olan, 
    // motor nesnesini yaratan fonksiyon deklarasyonu. 
    // (Bunu daha sonra "Entry Point" mantığına bölebiliriz)
    Application* CreateApplication();

} // namespace BSA
