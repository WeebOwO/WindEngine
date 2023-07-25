#include "GLTFLoader.h"
#include "Runtime/Resource/GLTFLoader.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace wind::gltf {
std::pair<glm::vec3, glm::vec3> ComputeTangentSpace(const glm::vec3& pos1, const glm::vec3& pos2,
                                                const glm::vec3& pos3, const glm::vec2& tex1,
                                                const glm::vec2& tex2, const glm::vec2& tex3) {
    // Edges of the triangle : postion delta
    auto deltaPos1 = pos2 - pos1;
    auto deltaPos2 = pos3 - pos1;

    // texture delta
    auto deltaT1 = tex2 - tex1;
    auto deltaT2 = tex3 - tex1;

    float r         = 1.0f / (deltaT1.x * deltaT2.y - deltaT1.y * deltaT2.x);
    auto  tangent   = (deltaPos1 * deltaT2.y - deltaPos2 * deltaT1.y) * r;
    auto  bitangent = (deltaPos2 * deltaT1.x - deltaPos1 * deltaT2.x) * r;

    return std::make_pair(glm::normalize(tangent), glm::normalize(bitangent));
}

static auto ComputeTangentsBitangents(std::span<GLTFShape::Index> indices,
                                      std::span<const glm::vec3>  positions,
                                      std::span<const glm::vec2>  texCoords) {
    std::vector<std::pair<glm::vec3, glm::vec3>> tangentsBitangents;
    tangentsBitangents.resize(positions.size(),
                              {glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}});

    assert(indices.size() % 3 == 0);
    for (size_t i = 0; i < indices.size(); i += 3) {
        const auto& position1 = positions[indices[i + 0]];
        const auto& position2 = positions[indices[i + 1]];
        const auto& position3 = positions[indices[i + 2]];

        const auto& texCoord1 = texCoords[indices[i + 0]];
        const auto& texCoord2 = texCoords[indices[i + 1]];
        const auto& texCoord3 = texCoords[indices[i + 2]];

        auto tangentBitangent =
            ComputeTangentSpace(position1, position2, position3, texCoord1, texCoord2, texCoord3);

        tangentsBitangents[indices[i + 0]].first += tangentBitangent.first;
        tangentsBitangents[indices[i + 0]].second += tangentBitangent.second;

        tangentsBitangents[indices[i + 1]].first += tangentBitangent.first;
        tangentsBitangents[indices[i + 1]].second += tangentBitangent.second;

        tangentsBitangents[indices[i + 2]].first += tangentBitangent.first;
        tangentsBitangents[indices[i + 2]].second += tangentBitangent.second;
    }

    for (auto& [tangent, bitangent] : tangentsBitangents) {
        if (tangent != glm::vec3{0.0f, 0.0f, 0.0f}) tangent = glm::normalize(tangent);
        if (bitangent != glm::vec3{0.0f, 0.0f, 0.0f}) bitangent = glm::normalize(bitangent);
    }
    return tangentsBitangents;
}
static Format ImageFormatFromGLTFImage(const tinygltf::Image& image) {
    if (image.component == 1) {
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE) return Format::R8_SNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) return Format::R8_UNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT) return Format::R16_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) return Format::R16_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT) return Format::R32_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) return Format::R32_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT) return Format::R32_SFLOAT;
    }
    if (image.component == 2) {
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE) return Format::R8G8_SNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) return Format::R8G8_UNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT) return Format::R16G16_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) return Format::R16G16_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT) return Format::R32G32_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) return Format::R32G32_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT) return Format::R32G32_SFLOAT;
    }
    if (image.component == 3) {
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE) return Format::R8G8B8_SNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) return Format::R8G8B8_UNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT) return Format::R16G16B16_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            return Format::R16G16B16_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT) return Format::R32G32B32_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) return Format::R32G32B32_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT) return Format::R32G32B32_SFLOAT;
    }
    if (image.component == 4) {
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_BYTE) return Format::R8G8B8A8_SNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
            return Format::R8G8B8A8_UNORM;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_SHORT) return Format::R16G16B16A16_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            return Format::R16G16B16A16_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_INT) return Format::R32G32B32A32_SINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            return Format::R32G32B32A32_UINT;
        if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT) return Format::R32G32B32A32_SFLOAT;
    }
    return Format::UNDEFINED;
}

static ImageData CreateStubTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ImageData{std::vector{r, g, b, a}, Format::R8G8B8A8_UNORM, 1, 1};
}

GLTFModelData GLTFLoader::LoadFromGLTF(const std::string& filepath) {
    GLTFModelData      result;
    tinygltf::TinyGLTF loader;
    tinygltf::Model    model;
    std::string        errorMessage, warningMessage;

    bool res = loader.LoadASCIIFromFile(&model, &errorMessage, &warningMessage, filepath);
    if (!res) std::cout << errorMessage;

    result.materials.reserve(model.materials.size());

    // collect material data
    for (const auto& material : model.materials) {
        auto& resultMaterial          = result.materials.emplace_back();
        resultMaterial.name           = material.name;
        resultMaterial.roughnessScale = material.pbrMetallicRoughness.roughnessFactor;
        resultMaterial.metallicScale  = material.pbrMetallicRoughness.metallicFactor;
        if (material.pbrMetallicRoughness.baseColorTexture.index != -1) {
            const auto& albedoTexture =
                model.images[model.textures[material.pbrMetallicRoughness.baseColorTexture.index]
                                 .source];
            resultMaterial.albedoTexture = ImageData{
                albedoTexture.image,
                ImageFormatFromGLTFImage(albedoTexture),
                (uint32_t)albedoTexture.width,
                (uint32_t)albedoTexture.height,
            };
        } else {
            // if don't have a basecolor, craete a white one
            resultMaterial.albedoTexture = CreateStubTexture(255, 255, 255, 255);
        }

        if (material.normalTexture.index != -1) {
            const auto& normalTexture =
                model.images[model.textures[material.normalTexture.index].source];
            resultMaterial.normalTexture = ImageData{
                normalTexture.image,
                ImageFormatFromGLTFImage(normalTexture),
                (uint32_t)normalTexture.width,
                (uint32_t)normalTexture.height,
            };
        } else {
            resultMaterial.normalTexture = CreateStubTexture(127, 127, 255, 255);
        }

        if (material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
            const auto& metallicRoughnessTexture =
                model.images
                    [model.textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index]
                         .source];
            resultMaterial.metallicRoughness = ImageData{
                metallicRoughnessTexture.image,
                ImageFormatFromGLTFImage(metallicRoughnessTexture),
                (uint32_t)metallicRoughnessTexture.width,
                (uint32_t)metallicRoughnessTexture.height,
            };
        } else {
            resultMaterial.metallicRoughness = CreateStubTexture(0, 255, 0, 255);
        }
    }

    // collect mesh data
    for (const auto& mesh : model.meshes) {
        result.shapes.reserve(result.shapes.size() + mesh.primitives.size());

        for (const auto& primitive : mesh.primitives) {
            auto& resultShape         = result.shapes.emplace_back();
            resultShape.name          = "shape_" + std::to_string(result.shapes.size());
            resultShape.materialIndex = (uint32_t)primitive.material;
            const auto& indexAccessor = model.accessors[primitive.indices];

            auto&          indexBuffer = model.bufferViews[indexAccessor.bufferView];
            const uint8_t* indexBegin =
                model.buffers[indexBuffer.buffer].data.data() + indexBuffer.byteOffset;

            const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& normalAccessor   = model.accessors[primitive.attributes.at("NORMAL")];

            const auto& positionBuffer = model.bufferViews[positionAccessor.bufferView];
            const auto& texCoordBuffer = model.bufferViews[texCoordAccessor.bufferView];
            const auto& normalBuffer   = model.bufferViews[normalAccessor.bufferView];

            const uint8_t* positionBegin =
                model.buffers[positionBuffer.buffer].data.data() + positionBuffer.byteOffset;
            const uint8_t* texCoordBegin =
                model.buffers[texCoordBuffer.buffer].data.data() + texCoordBuffer.byteOffset;
            const uint8_t* normalBegin =
                model.buffers[normalBuffer.buffer].data.data() + normalBuffer.byteOffset;

            std::span<glm::vec3> positions((glm::vec3*)positionBegin,
                                           (glm::vec3*)(positionBegin + positionBuffer.byteLength));
            std::span<glm::vec2> texCoords((glm::vec2*)texCoordBegin,
                                           (glm::vec2*)(texCoordBegin + texCoordBuffer.byteLength));
            std::span<glm::vec3> normals((glm::vec3*)normalBegin,
                                         (glm::vec3*)(normalBegin + normalBuffer.byteLength));

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                std::span<const uint16_t> indices(
                    (const uint16_t*)indexBegin,
                    (const uint16_t*)(indexBegin + indexBuffer.byteLength));
                resultShape.indices.resize(indices.size());
                for (size_t i = 0; i < resultShape.indices.size(); i++)
                    resultShape.indices[i] = (GLTFShape::Index)indices[i];
            }
            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                std::span<const uint32_t> indices(
                    (const uint32_t*)indexBegin,
                    (const uint32_t*)(indexBegin + indexBuffer.byteLength));
                resultShape.indices.resize(indices.size());
                for (size_t i = 0; i < resultShape.indices.size(); i++)
                    resultShape.indices[i] = (GLTFShape::Index)indices[i];
            }
            auto tangentsBitangents =
                ComputeTangentsBitangents(resultShape.indices, positions, texCoords);

            resultShape.vertices.resize(positions.size());

            for (size_t i = 0; i < resultShape.vertices.size(); i++) {
                auto& vertex     = resultShape.vertices[i];
                vertex.position  = positions[i];
                vertex.texcoord  = texCoords[i];
                vertex.Normal    = normals[i];
                vertex.tangent   = tangentsBitangents[i].first;
                vertex.bitangent = tangentsBitangents[i].second;
            }
        }
    }
    
    return result;
}

}; // namespace wind::gltf