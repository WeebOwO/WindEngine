#pragma once

#include <glm/glm.hpp>

namespace wind {

struct DirectionalLight {
    glm::vec3 lightPos{};
    glm::vec3 direction{};
    glm::vec3 radiance{1.0f};
    glm::vec3 ligthColor{1.0f};
};

struct PointLight {
    float lightRadius;
    float fallScale;
};

struct SpotLight {};

} // namespace wind