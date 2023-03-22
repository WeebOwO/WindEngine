#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <string_view>

namespace wind {
class Window {
public:
    Window(uint32_t width, uint32_t height, std::string_view title);
    ~Window();
    auto GetWindow() { return m_window; }

private:
    struct WindowInfo {
        uint32_t    width, height;
        std::string title;
    };
    GLFWwindow* m_window;
    WindowInfo  m_windowInfo;
};
} // namespace wind