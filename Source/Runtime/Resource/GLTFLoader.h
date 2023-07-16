#pragma once

#include <string>

#include "glm/glm.hpp"

#include "Runtime/Resource/ImageData.h"

namespace wind::gltf {
// this part is mainly come from https://github.com/asc-community/VulkanAbstractionLayer
struct GLTFVertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 Normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct GLTFMaterial {
    std::string Name;
    ImageData   AlbedoTexture;
    ImageData   NormalTexture;
    ImageData   MetallicRoughness;
    float       RoughnessScale = 1.0f;
    float       MetallicScale  = 1.0f;
};

struct GLTFShape {
    using Index = uint32_t;
    std::string             Name;
    std::vector<GLTFVertex> Vertices;
    std::vector<Index>      Indices;
    uint32_t                MaterialIndex = -1;
};

struct GLTFModelData {
    std::vector<GLTFShape>    shapes;
    std::vector<GLTFMaterial> materials;
};

class GLTFLoader {
public:
    static GLTFModelData LoadFromGLTF(const std::string& filepath);
};
} // namespace wind::gltf