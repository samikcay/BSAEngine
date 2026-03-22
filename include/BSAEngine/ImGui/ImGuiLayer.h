#pragma once

#include "BSAEngine/Events/Event.h"

namespace BSA {

    class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void Init();
        void Shutdown();
        
        // Frame basinda ImGui cizimlerine hazirlik yapar
        void Begin();
        // Frame sonunda ImGui cizimlerini ekrana gonderir
        void End();

        // Mouse/Klavye inputlarini ImGui'ye aktarmak icin
        void OnEvent(Event& e);

    private:
        bool m_BlockEvents = true; // ImGui arayuzu uzerindeyken oyunun tiklamalari almasini engeller
    };

}