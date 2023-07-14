#pragma once

#include "glm/glm.hpp"

#include <vulkan/vulkan.hpp>

namespace wind {
struct GPUIndirectObject {
    vk::DrawIndexedIndirectCommand drawCommand;
    uint32_t                       objectID;
    uint32_t                       batchID;
};

struct DrawMesh {
    uint32_t firstVertex;
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t vertexCount;
    bool     isMerged;
};

struct RenderBound {
    glm::vec3 origin;
    float     radius;
    glm::vec3 extents;
    bool      valid;
};

class GPUScene {    
public:

private:
    
};
}; // namespace wind