#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Runtime/Base/Macro.h"
#include "Runtime/Input/Input.h"
#include "Runtime/Input/KeyCodes.h"

namespace wind {
FirstPersonCamera::FirstPersonCamera(float initVerticalFOV, float initNearClip, float initFarClip) {
    nearClip    = initNearClip;
    farClip     = initFarClip;
    verticalFOV = initVerticalFOV;

    m_forwardDirection = glm::vec3(0, 0, -1);
    position           = glm::vec3(0, 0, 6);
}
constexpr float FirstPersonCamera::GetRotationSpeed() { return 0.3f; }

bool FirstPersonCamera::OnUpdate(float ts) {
    static auto inputManger = InputManger::GetInstance();
    glm::vec2   mousePos    = inputManger->GetMousePosition();
    glm::vec2   delta       = (mousePos - lastMousePosition) * 0.002f;
    lastMousePosition       = mousePos;

    if (!inputManger->IsMouseButtonDown(MouseButton::Right)) {
        inputManger->SetCursorMode(CursorMode::Normal);
        return false;
    }

    inputManger->SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    glm::vec3           rightDirection = glm::cross(m_forwardDirection, upDirection);

    float speed = 5.0f;

    // Movement
    if (inputManger->IsKeyDown(KeyCode::W)) {
        position += m_forwardDirection * speed * ts;
        moved = true;
    } else if (inputManger->IsKeyDown(KeyCode::S)) {
        position -= m_forwardDirection * speed * ts;
        moved = true;
    }
    if (inputManger->IsKeyDown(KeyCode::A)) {
        position -= rightDirection * speed * ts;
        moved = true;
    } else if (inputManger->IsKeyDown(KeyCode::D)) {
        position += rightDirection * speed * ts;
        moved = true;
    }
    if (inputManger->IsKeyDown(KeyCode::Q)) {
        position -= upDirection * speed * ts;
        moved = true;
    } else if (inputManger->IsKeyDown(KeyCode::E)) {
        position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = delta.y * GetRotationSpeed();
        float yawDelta   = delta.x * GetRotationSpeed();

        glm::quat q =
            glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                                      glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        m_forwardDirection = glm::rotate(q, m_forwardDirection);

        moved = true;
    }

    if (moved) RecalculateView();

    return moved;
}

void FirstPersonCamera::OnResize(uint32_t width, uint32_t height) {
    if (width == viewportWidth && height == viewportHeight) return;

    viewportWidth  = width;
    viewportHeight = height;

    RecalculateProjection();
}

void FirstPersonCamera::RecalculateView() {
    view        = glm::lookAt(position, position + m_forwardDirection, glm::vec3(0, 1, 0));
    inverseView = glm::inverse(view);
}

void FirstPersonCamera::RecalculateProjection() {
    projection = glm::perspectiveFov(glm::radians(verticalFOV), (float)viewportWidth,
                                     (float)viewportHeight, nearClip, farClip);
    projection[1][1] *= -1.0f;
    inverseProjection = glm::inverse(projection);
}

// Not good plan
OrbitCamera::OrbitCamera(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, this);

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        OrbitCamera* camera = reinterpret_cast<OrbitCamera*>(glfwGetWindowUserPointer(window));
        camera->m_viewSetting.distance += camera->m_zoomSpeed * -yoffset;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        OrbitCamera*    camera  = reinterpret_cast<OrbitCamera*>(glfwGetWindowUserPointer(window));
        const InputMode oldmode = camera->m_inputMode;
        if (action == GLFW_PRESS && camera->m_inputMode == InputMode::None) {
            switch (button) {
            case GLFW_MOUSE_BUTTON_1:
                camera->m_inputMode = InputMode::RotatingView;
                break;
            case GLFW_MOUSE_BUTTON_2:
                camera->m_inputMode = InputMode::RotatingScene;
                break;
            }
        }
        if (action == GLFW_RELEASE &&
            (button == GLFW_MOUSE_BUTTON_1 || button == GLFW_MOUSE_BUTTON_2)) {
            camera->m_inputMode = InputMode::None;
        }
        if (oldmode != camera->m_inputMode) {
            if (camera->m_inputMode == InputMode::None) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwGetCursorPos(window, &camera->m_prevCursorX, &camera->m_prevCursorY);
            }
        }
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        OrbitCamera* camera = reinterpret_cast<OrbitCamera*>(glfwGetWindowUserPointer(window));

        if (camera->m_inputMode != InputMode::None) {
            const double dx = xpos - camera->m_prevCursorX;
            const double dy = ypos - camera->m_prevCursorY;

            switch (camera->m_inputMode) {
            case InputMode::RotatingView:
                camera->m_viewSetting.yaw += camera->m_orbitSpeed * float(dx);
                camera->m_viewSetting.pitch += camera->m_orbitSpeed * float(dy);
                break;
            default:
                break;
            }

            camera->m_prevCursorX = xpos;
            camera->m_prevCursorY = ypos;
        }
    });
}

bool OrbitCamera::OnUpdate(float ts) {
    RecalculateView();
    return true;
}

void OrbitCamera::RecalculateProjection() {
    projection = glm::perspectiveFov(m_viewSetting.fov, (float)viewportWidth, float(viewportHeight),
                                     nearClip, farClip);
    projection[1][1] *= -1.0f;

    inverseProjection = glm::inverse(projection);
}

void OrbitCamera::RecalculateView() {
    glm::mat4 viewRotationMatrix =
        glm::eulerAngleXY(glm::radians(m_viewSetting.pitch), glm::radians(m_viewSetting.yaw));
    view =
        glm::translate(glm::mat4{1.0f}, {0.0f, 0.0f, -m_viewSetting.distance}) * viewRotationMatrix;
    position    = glm::inverse(view)[3];
    inverseView = glm::inverse(view);
}

void OrbitCamera::OnResize(uint32_t width, uint32_t height) {
    if (width == viewportWidth && height == viewportHeight) return;
    viewportWidth  = width;
    viewportHeight = height;
    RecalculateProjection();
}
} // namespace wind