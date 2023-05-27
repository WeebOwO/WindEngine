#include "Input.h"
#include "Runtime/Base/Macro.h"

namespace wind {
std::shared_ptr<InputManger> InputManger::s_instance {nullptr};

void InputManger::Init(GLFWwindow* window) { 
    WIND_CORE_INFO("InputManger init");
    s_instance = std::make_shared<InputManger>(window);
}

glm::vec2 InputManger::GetMousePosition() {
    double x, y;
    glfwGetCursorPos(m_windowHandle, &x, &y);
    return {(float)x, (float)y};
}

bool InputManger::IsKeyDown(KeyCode keycode) {
    int state = glfwGetKey(m_windowHandle, (int)keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool InputManger::IsMouseButtonDown(MouseButton button) {
    int state = glfwGetMouseButton(m_windowHandle, (int)button);
    return state == GLFW_PRESS;
}

void InputManger::SetCursorMode(CursorMode mode) {
    glfwSetInputMode(m_windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

} // namespace wind