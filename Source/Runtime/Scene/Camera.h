#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <GLFW/glfw3.h>

namespace wind {

struct BaseCamera {
    [[nodiscard]] const glm::mat4& GetProjection() { return projection; }
    [[nodiscard]] const glm::mat4& GetView() { return view; }
    [[nodiscard]] const glm::mat4& GetInverseProjection() const { return inverseProjection; }
    [[nodiscard]] const glm::mat4& GetInverseView() const { return inverseView; }
    [[nodiscard]] const glm::vec3& GetPosition() const { return position; }

    virtual bool OnUpdate(float ts)                        = 0;
    virtual void OnResize(uint32_t width, uint32_t height) = 0;

    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 inverseProjection{1.0f};
    glm::mat4 inverseView{1.0f};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec2 lastMousePosition{0.0f, 0.0f};

    float verticalFOV = 45.0f;

    float nearClip = 0.1f;
    float farClip  = 1000.0f;

    uint32_t viewportWidth = 0, viewportHeight = 0;
};

class FirstPersonCamera : public BaseCamera {
public:
    FirstPersonCamera(float verticalFOV, float nearClip, float farClip);

    bool OnUpdate(float ts) override;
    void OnResize(uint32_t width, uint32_t height) override;

    [[nodiscard]] const glm::vec3& GetDirection() const { return m_forwardDirection; }

    constexpr float GetRotationSpeed();

private:
    void RecalculateProjection();
    void RecalculateView();

private:
    glm::vec3 m_forwardDirection{0.0f, 0.0f, 0.0f};
    float     m_speed = 1000.0f;
};

class OrbitCamera : public BaseCamera {
public:
    OrbitCamera(GLFWwindow* window);

    bool OnUpdate(float ts) override;
    void OnResize(uint32_t width, uint32_t height) override;

    auto& GetViewSetting() const { return m_viewSetting; }

private:
    void RecalculateProjection();
    void RecalculateView();

private:
    float m_orbitSpeed = 1.0f;
    float m_zoomSpeed  = 4.0f;

    enum class InputMode : uint8_t { None, RotatingView, RotatingScene };

    struct ViewSettings {
        float pitch    = 0.0f;
        float yaw      = 0.0f;
        float distance = 150.0f;
        float fov      = 45.0f;
    };

    struct SceneSettings {
        float pitch = 0.0f;
        float yaw   = 0.0f;
    };

    double m_prevCursorX;
    double m_prevCursorY;

    ViewSettings  m_viewSetting;
    SceneSettings m_sceneSetting;
    InputMode     m_inputMode = InputMode::None;
};
} // namespace wind
