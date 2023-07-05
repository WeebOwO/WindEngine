#include "Runtime/Scene/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Runtime/Base/Macro.h"
#include "Runtime/Input/Input.h"

namespace wind {
FirstPersonCamera::FirstPersonCamera(float verticalFOV, float nearClip, float farClip)
    : m_verticalFOV(verticalFOV), m_nearClip(nearClip), m_farClip(farClip) {
    m_forwardDirection = glm::vec3(0, 0, -1);
    m_position         = glm::vec3(0, 0, 6);
}
constexpr float FirstPersonCamera::GetRotationSpeed() { return 0.3f; }

bool FirstPersonCamera::OnUpdate(float ts) {
    auto      InputManger = InputManger::GetInstance();
    glm::vec2 mousePos    = InputManger->GetMousePosition();
    glm::vec2 delta       = (mousePos - m_lastMousePosition) * 0.002f;
    m_lastMousePosition   = mousePos;

    if (!InputManger->IsMouseButtonDown(MouseButton::Right)) {
        InputManger->SetCursorMode(CursorMode::Normal);
        return false;
    }

    InputManger->SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    glm::vec3           rightDirection = glm::cross(m_forwardDirection, upDirection);

    float speed = 5.0f;

    // Movement
    if (InputManger->IsKeyDown(KeyCode::W)) {
        m_position += m_forwardDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::S)) {
        m_position -= m_forwardDirection * speed * ts;
        moved = true;
    }
    if (InputManger->IsKeyDown(KeyCode::A)) {
        m_position -= rightDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::D)) {
        m_position += rightDirection * speed * ts;
        moved = true;
    }
    if (InputManger->IsKeyDown(KeyCode::Q)) {
        m_position -= upDirection * speed * ts;
        moved = true;
    } else if (InputManger->IsKeyDown(KeyCode::E)) {
        m_position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = -delta.y * GetRotationSpeed();
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
    if (width == m_viewportWidth && height == m_viewportHeight) return;

    m_viewportWidth  = width;
    m_viewportHeight = height;

    RecalculateProjection();
}

void FirstPersonCamera::RecalculateView() {
    m_view        = glm::lookAt(m_position, m_position + m_forwardDirection, glm::vec3(0, 1, 0));
    m_inverseView = glm::inverse(m_view);
}

void FirstPersonCamera::RecalculateProjection() {
    m_projection        = glm::perspectiveFov(glm::radians(m_verticalFOV), (float)m_viewportWidth,
                                              (float)m_viewportHeight, m_nearClip, m_farClip);
    m_inverseProjection = glm::inverse(m_projection);
}
} // namespace wind