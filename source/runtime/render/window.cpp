#include "window.h"

#include "GLFW/glfw3.h"

Window::Window(uint32_t width, uint32_t height, std::string_view title)
    : m_width(width), m_height(height), m_title(title) {
    InitWindow();
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::InitWindow() {
    glfwInit();
    // tell the glfw not to create opengl context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
}
