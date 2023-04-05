#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace wind {
    struct TransfromComponent {
       alignas(16) glm::mat4 model;
       alignas(16) glm::mat4 view;
       alignas(16) glm::mat4 projection;    
    };
}