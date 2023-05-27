#pragma once

#include <glm/glm.hpp>

#include "GLFW/glfw3.h"

#include "Runtime/Base/Macro.h"
#include "Runtime/Input/KeyCodes.h"

namespace wind {
class InputManger {
public:
    PERMIT_COPY(InputManger);
    PERMIT_MOVE(InputManger);

    InputManger(GLFWwindow* window) : m_windowHandle(window) {}

    bool IsKeyDown(KeyCode keycode);
    bool IsMouseButtonDown(MouseButton button);

    glm::vec2 GetMousePosition();
    void      SetCursorMode(CursorMode mode);

    [[nodiscard]] static auto GetInstance() { return s_instance; }

    static void Init(GLFWwindow* window);

private:
    GLFWwindow*                         m_windowHandle;
    static std::shared_ptr<InputManger> s_instance;
};

} // namespace wind