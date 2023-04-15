#pragma once

namespace wind {
struct Camera {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    float viewDistance {50.0f};
    float orbitSpeed {1.0f};
    float zoomSpeed {4.0f};

    float znear {1.0f};
    float zfar {1000.0f};
    float vFov {45.0f}; // radians 
};
} // namespace wind