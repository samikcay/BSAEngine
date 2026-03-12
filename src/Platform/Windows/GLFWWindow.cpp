#include "BSAEngine/Log/Log.h"
#include "BSAEngine/Core/Window.h"
#include "GLFWWindow.h"
#include "BSAEngine/Events/ApplicationEvent.h"
#include "BSAEngine/Events/MouseEvent.h"
#include "BSAEngine/Events/KeyEvent.h"

namespace BSA {

    static bool s_GLFWInitialized = false;

    // GLFW Hata Callback'i (Error Callback)
    static void GLFWErrorCallback(int error, const char* description) {
        BSA_ENGINE_ERROR("GLFW Hatasi ({0}): {1}", error, description);
    }

    Window* Window::Create(const WindowProps& props) {
        // İlerde buraya farklı platformlar için (Örn: Direct3D, Vulkan için özel yüzeyler
        // veya MacOS için Metal) farklı pencere nesneleri dönecek şekilde mantık eklenebilir.
        // Şimdilik sadece Windows (ve GLFW destekleyen diğer platformlar) için GLFW döndürüyoruz.
        return new GLFWWindow(props);
    }

    GLFWWindow::GLFWWindow(const WindowProps& props) {
        Init(props);
    }

    GLFWWindow::~GLFWWindow() {
        Shutdown();
    }

    void GLFWWindow::Init(const WindowProps& props) {
        m_Data.Title  = props.Title;
        m_Data.Width  = props.Width;
        m_Data.Height = props.Height;

        BSA_ENGINE_INFO("Pencere olusturuluyor: {0} ({1}x{2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized) {
            int success = glfwInit();
            // assert(success && "GLFW baslatilamadi!");
            if (!success) {
                BSA_ENGINE_CRITICAL("GLFW baslatilamadi!");
                return;
            }

            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        
        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data); // Callbacks için veri aktarımı
        SetVSync(true); // Varsayılan olarak VSync açık

        // -----------------------------------------------------------
        // GLFW Callback'leri
        // -----------------------------------------------------------

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    // GLFW, repeat count tutmuyor, biz 1 veriyoruz (veya kendi sistemimizde sayabiliriz)
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void GLFWWindow::Shutdown() {
        glfwDestroyWindow(m_Window);
        // Motor tamamen kapanırken glfwTerminate çağırmak daha sağlıklıdır, 
        // ancak şimdilik basitlik için sadece pencereyi siliyoruz.
    }

    void GLFWWindow::OnUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void GLFWWindow::SetVSync(bool enabled) {
        if (enabled) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }

        m_Data.VSync = enabled;
    }

    bool GLFWWindow::IsVSync() const {
        return m_Data.VSync;
    }

} // namespace BSA
