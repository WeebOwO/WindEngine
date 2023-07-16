#include "GLTFLoader.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace wind::gltf {
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

    for (const auto& material : model.materials) {
        auto& resultMaterial          = result.materials.emplace_back();
        resultMaterial.Name           = material.name;
        resultMaterial.RoughnessScale = material.pbrMetallicRoughness.roughnessFactor;
        resultMaterial.MetallicScale  = material.pbrMetallicRoughness.metallicFactor;
        if (material.pbrMetallicRoughness.baseColorTexture.index != -1) {
            const auto& albedoTexture =
                model.images[model.textures[material.pbrMetallicRoughness.baseColorTexture.index]
                                 .source];
            resultMaterial.AlbedoTexture = ImageData{
                albedoTexture.image,
                ImageFormatFromGLTFImage(albedoTexture),
                (uint32_t)albedoTexture.width,
                (uint32_t)albedoTexture.height,
            };
        } else {
            // if don't have a basecolor, craete a white one
            resultMaterial.AlbedoTexture = CreateStubTexture(255, 255, 255, 255);
        }

        if (material.normalTexture.index == -1) {
            const auto& normalTexture =
                model.images[model.textures[material.normalTexture.index].source];
            resultMaterial.NormalTexture = ImageData{
                normalTexture.image,
                ImageFormatFromGLTFImage(normalTexture),
                (uint32_t)normalTexture.width,
                (uint32_t)normalTexture.height,
            };
        } else {
            resultMaterial.NormalTexture = CreateStubTexture(127, 127, 255, 255);
        }
        
    }

    return result;
}

}; // namespace wind::gltf