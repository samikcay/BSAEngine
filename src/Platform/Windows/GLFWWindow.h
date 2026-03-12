#pragma once

#include "BSAEngine/Core/Window.h"
#include <GLFW/glfw3.h>

namespace BSA {

    // ---------------------------------------------------------------
    // GLFW spesifik pencere implementasyonu
    // ---------------------------------------------------------------
    class GLFWWindow : public Window {
    public:
        GLFWWindow(const WindowProps& props);
        virtual ~GLFWWindow();

        void OnUpdate() override;

        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }

        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline void* GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    private:
        GLFWwindow* m_Window;

        // GLFW callback'lerine aktarabilmek için sınıf verilerini paketleriz
        struct WindowData {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;
        };

        WindowData m_Data;
    };

} // namespace BSA
