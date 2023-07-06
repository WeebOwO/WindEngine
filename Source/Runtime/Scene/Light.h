#pragma once

#include <glm/glm.hpp>

namespace wind {

struct DirectionalLight {
    alignas(16) glm::vec3 direction {};
    alignas(16) glm::vec3 intensity {10.0f, 10.0f, 10.0f};
    alignas(16) glm::vec3 ligthColor{1.0f, 1.0f, 1.0f};
};

struct PointLight {
    float lightRadius;
    float fallScale;
};

struct SpotLight {
    
};

} // namespace wind