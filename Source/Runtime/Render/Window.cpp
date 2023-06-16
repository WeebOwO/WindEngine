#include "Window.h"

namespace wind {

Window::Window(uint32_t width, uint32_t height, std::string_view title): m_windowInfo(WindowInfo{width, height, std::string(title)}) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
}

Window::~Window() { glfwDestroyWindow(m_window); }

void Window::OnUpdate(float fs) {
    glfwPollEvents();
}
} // namespace wind