#pragma once

#include "BSAEngine/Core/Window.h"
#include <memory>

namespace BSA {

    // ---------------------------------------------------------------
    // Motorun Ana Sınıfı (Application)
    // Game Loop'u (Oyun Döngüsü) yönetir, Pencere'ye (Window) sahiptir.
    // ---------------------------------------------------------------
    class Event;
    class WindowCloseEvent;

    class Application {
    public:
        Application();
        virtual ~Application();

        // Motorun ana döngüsünü başlatır
        void Run();

        // Pencereden veya başka yerlerden gelen olayları işler
        void OnEvent(Event& e);

        // Motoru güvenli şekilde kapatmak için
        void Close() { m_Running = false; }

        inline Window& GetWindow() { return *m_Window; }
        
        static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;

        static Application* s_Instance;
    };

    // Client (Oyun) tarafı tarafından overload edilecek olan, 
    // motor nesnesini yaratan fonksiyon deklarasyonu. 
    // (Bunu daha sonra "Entry Point" mantığına bölebiliriz)
    Application* CreateApplication();

} // namespace BSA
