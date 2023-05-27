#include "Runtime/Scene/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Runtime/Base/Macro.h"
#include "Runtime/Input/Input.h"

namespace wind {
Camera::Camera(float verticalFOV, float nearClip, float farClip)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip) {
    m_ForwardDirection = glm::vec3(0, 0, -1);
    m_Position         = glm::vec3(0, 0, 6);
}
float Camera::GetRotationSpeed() { return 0.3f; }

bool  Camera::OnUpdate(float ts) {
    auto      InputManger = InputManger::GetInstance();
    glm::vec2 mousePos    = InputManger->GetMousePosition();
    glm::vec2 delta       = (mousePos - m_LastMousePosition) * 0.002f;
    m_LastMousePosition   = mousePos;

    if (!InputManger->IsMouseButtonDown(MouseButton::Right)) {
        InputManger->SetCursorMode(CursorMode::Normal);
        return false;
    }

    InputManger->SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    glm::vec3           rightDirection = glm::cross(m_ForwardDirection, upDirection);

    float speed = 5.0f;

    // Movement
    if (InputManger->IsKeyDown(KeyCode::W)) {
        WIND_INFO("Move forward");
        m_Position += m_ForwardDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::S)) {
        WIND_INFO("Move backward");
        m_Position -= m_ForwardDirection * speed * ts;
        moved = true;
    }
    if (InputManger->IsKeyDown(KeyCode::A)) {
        WIND_INFO("Move left");
        m_Position -= rightDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::D)) {
        WIND_INFO("Move right");
        m_Position += rightDirection * speed * ts;
        moved = true;
    }
    if (InputManger->IsKeyDown(KeyCode::Q)) {
        m_Position -= upDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::E)) {
        m_Position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = delta.y * GetRotationSpeed();
        float yawDelta   = delta.x * GetRotationSpeed();

        glm::quat q =
            glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                                       glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

        moved = true;
    }

    if (moved) RecalculateView();

    return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height) {
    if (width == m_ViewportWidth && height == m_ViewportHeight) return;

    m_ViewportWidth  = width;
    m_ViewportHeight = height;

    RecalculateProjection();
}

void Camera::RecalculateView() {
    m_View        = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
    m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateProjection() {
    m_Projection        = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth,
                                              (float)m_ViewportHeight, m_NearClip, m_FarClip);
    m_InverseProjection = glm::inverse(m_Projection);
}
} // namespace wind