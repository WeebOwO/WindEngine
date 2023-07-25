#pragma once

#include <string>

#include "glm/glm.hpp"

#include "Runtime/Render/RHI/Buffer.h"
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
    std::string name;
    ImageData   albedoTexture;
    ImageData   normalTexture;
    ImageData   metallicRoughness;
    float       roughnessScale = 1.0f;
    float       metallicScale  = 1.0f;
};

struct GLTFShape {
    using Index = uint32_t;
    std::string             name;
    std::vector<GLTFVertex> vertices;
    std::vector<Index>      indices;
    uint32_t                materialIndex = -1;
};

struct GLTFModelData {
    std::vector<GLTFShape>    shapes;
    std::vector<GLTFMaterial> materials;
};

struct GLTFMesh {
    struct Material {
        uint32_t albedoIndex;
        uint32_t normalIndex;
        uint32_t metallicRoughnessIndex;
        float    roughnessScale;
        float    metallicScale;
        uint32_t padding[3];
    };

    struct Submesh {
        Buffer   vertexBuffer;
        Buffer   indexBuffer;
        uint32_t materialIndex;
    };

    std::vector<Submesh>  submeshes;
    std::vector<Material> materials;
    std::vector<Image>    textures;

    struct MeshData {
        glm::mat4 Transform = glm::mat4(1.0f);
    } Data;

};

class GLTFLoader {
public:
    static GLTFModelData LoadFromGLTF(const std::string& filepath);
    void                 PackToGLTFMesh(const GLTFModelData& source, GLTFMesh& Mesh);
};
} // namespace wind::gltf