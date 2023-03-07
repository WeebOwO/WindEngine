#pragma once

#include "runtime/macro.h"
#define GLFW_INCLUDE_VULKAN
#include <string>

#include <GLFW/glfw3.h>

#include "macro.h"

namespace wind {
class Window {
public:
    PERMIT_COPY(Window);    
    Window(uint32_t width, uint32_t height, const std::string& title);
    bool ShouldClose() {return glfwWindowShouldClose(m_window);}
    ~Window();
private:
    void InitWindow();
    uint32_t m_width, m_height;
    const char* m_windowName = "Simple Vulkan Engine";
    GLFWwindow* m_window;
};
}