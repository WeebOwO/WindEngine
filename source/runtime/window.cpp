#include "window.h"
#include "GLFW/glfw3.h"

namespace wind {
    Window::Window(uint32_t width, uint32_t height, const std::string& title):
    m_width(width), m_height(height), m_windowName(title.c_str()) {
        InitWindow();
    }

    void Window::InitWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(m_width, m_height, m_windowName, nullptr, nullptr); 
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}