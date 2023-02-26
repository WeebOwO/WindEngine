#pragma once

#include <cstdint>
#include <string_view>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "runtime/base/macro.h"

class Window {
public:
    DELETE_COPY(Window)
    DELETE_MOVE(Window)
    ~Window();
    Window(uint32_t width, uint32_t height, std::string_view title);
    bool ShouldClose() {return glfwWindowShouldClose(m_window);}
private:
    void InitWindow();
private:
    const uint32_t m_width {800};
    const uint32_t m_height {600};
    const std::string m_title {"VulkanEngine"};
    GLFWwindow* m_window {nullptr};
};
