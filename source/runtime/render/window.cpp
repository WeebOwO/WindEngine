#include "runtime/render/window.h"

#include "GLFW/glfw3.h"

#include "runtime/base/macro.h"
#include "runtime/resource/scene.h"

namespace wind {

Window::Window(uint32_t width, uint32_t height, std::string_view title) {
    m_windowInfo.width  = width;
    m_windowInfo.height = height;
    m_windowInfo.title  = title;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        auto  self   = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        auto& camera = Scene::GetWorld().GetCamera();
        // WIND_CORE_INFO("Current MouseX is {}, MouseY is {}", xpos, ypos);
        if (self->m_mode != InputMode::None) {
            const double dx = xpos - self->m_prevCursorX;
            const double dy = ypos - self->m_prevCursorY;

            if (self->m_mode == InputMode::RotatingView) {
                camera.yaw += camera.orbitSpeed * float(dx);
                camera.pitch += camera.orbitSpeed * float(dy);
            }
        }

        self->m_prevCursorX = xpos;
        self->m_prevCursorY = ypos;
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto            self    = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        const InputMode oldMode = self->m_mode;

        if (action == GLFW_PRESS && self->m_mode == InputMode::None) {
            switch (button) {
            case GLFW_MOUSE_BUTTON_1: {
                self->m_mode = InputMode::RotatingView;
                break;
            }
            }
        }

        if (action == GLFW_RELEASE &&
            (button == GLFW_MOUSE_BUTTON_1 || button == GLFW_MOUSE_BUTTON_2)) {
            self->m_mode = InputMode::None;
        }
        
        if (oldMode != self->m_mode) {
            if (self->m_mode == InputMode::None) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwGetCursorPos(window, &self->m_prevCursorX, &self->m_prevCursorY);
            }
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto  self   = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        auto& camera = Scene::GetWorld().GetCamera();
        camera.viewDistance += camera.zoomSpeed * float(-yoffset);
    });
}

Window::~Window() { glfwDestroyWindow(m_window); }
} // namespace wind