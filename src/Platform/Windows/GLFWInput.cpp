#include "GLFWInput.h"
#include "BSAEngine/Core/Application.h"
#include <GLFW/glfw3.h>
#include <ostream>
#include <string_view>

namespace BSA {

    // Singleton (Tekil) Input nesnesi başlangıcı
    Input* Input::s_Instance = new GLFWInput();

    bool GLFWInput::IsKeyPressedImpl(int keycode) {
        // Otomatik olarak o anki aktif pencereyi çek (Engine'in sahip olduğu pencere)
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        
        // GLFW key state sorgulaması
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button) {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> GLFWInput::GetMousePositionImpl() {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        return { (float)xpos, (float)ypos };
    }

    float GLFWInput::GetMouseXImpl() {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    float GLFWInput::GetMouseYImpl() {
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

} // namespace BSA
