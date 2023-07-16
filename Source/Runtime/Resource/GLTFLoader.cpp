#include "GLTFLoader.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace wind::gltf {
GLTFModelData GLTFLoader::LoadFromGLTF(const std::string& filepath) {
    GLTFModelData result;
    tinygltf::TinyGLTF loader;
    tinygltf::Model    model;
    std::string        errorMessage, warningMessage;

    bool res = loader.LoadASCIIFromFile(&model, &errorMessage, &warningMessage, filepath);
    if(!res) std::cout << errorMessage;

    result.materials.reserve(model.materials.size());

    for(const auto& material : model.materials) {
        auto& resultMaterial = result.materials.emplace_back();
        
    }
    return result;
}

}; // namespace wind::gltf