#include "runtime/render/window.h"

namespace wind {
    Window::Window(uint32_t width, uint32_t height, std::string_view title) {
        m_windowInfo.width = width;
        m_windowInfo.height = height;
        m_windowInfo.title = title;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);  
    }
    Window::~Window() {
        glfwDestroyWindow(m_window);
    }
}