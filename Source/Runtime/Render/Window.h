#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <string_view>

#include <GLFW/glfw3.h>

namespace wind {
class Window {
public:
    friend class Renderer;
    Window(uint32_t width, uint32_t height, std::string_view title);
    ~Window();
    
    [[nodiscard]] auto GetWindow() const { return m_window; }
    [[nodiscard]] auto width() const { return m_windowInfo.width; }
    [[nodiscard]] auto height() const { return m_windowInfo.height; }

    void OnUpdate();
private:    
    enum class InputMode : uint8_t {
        None,
        RotatingView,
    };

    struct WindowInfo {
        uint32_t    width, height;
        std::string title;
    };

    GLFWwindow* m_window {nullptr};
    WindowInfo  m_windowInfo {};

    float       m_lastFrameTime {0.0f};
};
} // namespace wind