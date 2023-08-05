#pragma once

#include <glm/glm.hpp>

namespace wind {

struct DirectionalLight {
    glm::vec3 lightPos{};
    glm::vec3 direction{};
    // todo: fix this def
    glm::vec3 radiance{1.0f};
    glm::vec3 ligthColor{1.0f};
};

// simple Pointlight
struct PointLight {
   alignas(16)  glm::vec3 position;    
   alignas(16)  glm::vec3 intensity;
   alignas(16)  glm::vec3 lightColor;
};

// todo
struct SpotLight {

};

} // namespace wind