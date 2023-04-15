#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <string_view>

namespace wind {
class Window {
public:
    friend class Renderer;
    Window(uint32_t width, uint32_t height, std::string_view title);
    ~Window();
    
    [[nodiscard]] auto GetWindow() const { return m_window; }
    [[nodiscard]] auto width() const { return m_windowInfo.width; }
    [[nodiscard]] auto height() const { return m_windowInfo.height; }

private:    
    enum class InputMode : uint8_t {
        None,
        RotatingView,
    };

    struct WindowInfo {
        uint32_t    width, height;
        std::string title;
    };

    double m_prevCursorX;
    double m_prevCursorY;

    InputMode m_mode {InputMode::None};
    GLFWwindow* m_window {nullptr};
    WindowInfo  m_windowInfo;

};
} // namespace wind