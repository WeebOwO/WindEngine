#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace wind {

struct BaseCamera {
    
};

class FirstPersonCamera : public BaseCamera {
public:
    FirstPersonCamera(float verticalFOV, float nearClip, float farClip);

    bool OnUpdate(float ts);
    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] const glm::mat4& GetProjection() const { return m_projection; }
    [[nodiscard]] const glm::mat4& GetInverseProjection() const { return m_inverseProjection; }
    [[nodiscard]] const glm::mat4& GetView() const { return m_view; }
    [[nodiscard]] const glm::mat4& GetInverseView() const { return m_inverseView; }

    [[nodiscard]] const glm::vec3& GetPosition() const { return m_position; }
    [[nodiscard]] const glm::vec3& GetDirection() const { return m_forwardDirection; }

    constexpr float GetRotationSpeed();

private:
    void RecalculateProjection();
    void RecalculateView();

private:
    glm::mat4 m_projection{1.0f};
    glm::mat4 m_view{1.0f};
    glm::mat4 m_inverseProjection{1.0f};
    glm::mat4 m_inverseView{1.0f};

    float m_verticalFOV = 45.0f;
    float m_nearClip    = 0.1f;
    float m_farClip     = 100.0f;

    glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_forwardDirection{0.0f, 0.0f, 0.0f};

    glm::vec2 m_lastMousePosition{0.0f, 0.0f};

    uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
};
} // namespace wind
