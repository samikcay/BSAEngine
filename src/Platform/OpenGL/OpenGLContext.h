#pragma once

#include "BSAEngine/Renderer/GraphicsContext.h"

// GLFWwindow forward declaration (include GLFW/glfw3.h kalabalığını önler)
struct GLFWwindow;

namespace BSA {

    // ---------------------------------------------------------------
    // OpenGL Spesifik Context Uygulaması
    // ---------------------------------------------------------------
    class OpenGLContext : public GraphicsContext {
    public:
        // OpenGL GLFW penceresine bağımlı olduğu için Handle (pointer) alır.
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow* m_WindowHandle;
    };

} // namespace BSA
