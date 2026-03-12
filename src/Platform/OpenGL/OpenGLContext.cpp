#include "OpenGLContext.h"
#include "BSAEngine/Log/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BSA {

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        // assert(windowHandle && "Window handle is null!");
        if (!windowHandle) {
            BSA_ENGINE_CRITICAL("OpenGLContext: GLFWwindow nesnesi bulunamadi!");
        }
    }

    void OpenGLContext::Init() {
        // GLFW penceresini mevcut (current) OpenGL context'i yap
        glfwMakeContextCurrent(m_WindowHandle);

        // GLAD'ı GLFW aracılığıyla yükle (OpenGL fonksiyon göstergelerini alır)
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        // assert(status && "GLAD baslatilamadi!");
        if (!status) {
            BSA_ENGINE_CRITICAL("GLAD yuklenemedi!");
        }

        // Başarılı bir şekilde yüklendiğinde sürücü ve ekran kartı bilgilerini yazdır
        BSA_ENGINE_INFO("OpenGL Info:");
        BSA_ENGINE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        BSA_ENGINE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        BSA_ENGINE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }

} // namespace BSA
