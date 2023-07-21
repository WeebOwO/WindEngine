#include "Scene.h"

#include <memory>

#include "Runtime/Base/Io.h"
#include "Runtime/Base/Utils.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Resource/GLTFLoader.h"
#include "Runtime/Resource/ImageLoader.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
SkyBox::SkyBox() {
    skyBoxImage           = std::make_shared<Image>();
    skyBoxIrradianceImage = std::make_shared<Image>();
}
void Scene::AddLightData(const DirectionalLight& directionalLight) {
    m_directionalLights.push_back(directionalLight);
}

void Scene::Init() { WIND_CORE_INFO("Scene is Init"); }

void Scene::LoadSkyBox(const std::string& skyBoxModelPath, const std::string& skyboxImagePath,
                       const std::string& irradianceImagePath) {
    m_skybox               = std::make_shared<SkyBox>();
    Model::Builder builder = io::LoadModelFromFilePath(skyBoxModelPath);
    ImageLoader::LoadCubemap(*m_skybox->skyBoxImage, Format::R8G8B8A8_UNORM, skyboxImagePath);
    m_skybox->skyBoxModel = std::make_shared<Model>(builder);
    ImageLoader::LoadCubemap(*m_skybox->skyBoxIrradianceImage, Format::R8G8B8A8_UNORM,
                             skyboxImagePath);
}

void Scene::LoadGLTFScene(const std::string& resourceName, std::string_view filePath) {
    gltf::GLTFModelData model =
        gltf::GLTFLoader::LoadFromGLTF(R"(..\..\..\..\Assets\Scene\Sponza\glTF\Sponza.gltf)");
    gltf::GLTFMesh mesh;

    auto& backend       = RenderBackend::GetInstance();
    auto& stageBuffer   = backend.GetStagingBuffer();
    auto& commandBuffer = backend.GetCurrentCommands();

    commandBuffer.Begin();
    for (const auto& shape : model.shapes) {
        auto& submesh = mesh.submeshes.emplace_back();
        // process vertexBuffer
        submesh.vertexBuffer.Init(shape.vertices.size() * sizeof(gltf::GLTFVertex),
                                  BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DESTINATION,
                                  MemoryUsage::GPU_ONLY);
        auto vertexAllocation = stageBuffer.Submit(utils::MakeView(shape.vertices));
        commandBuffer.CopyBuffer(BufferInfo{stageBuffer.GetBuffer(), vertexAllocation.Offset},
                                 BufferInfo{submesh.vertexBuffer, 0}, vertexAllocation.Size);
        // process index buffer
        submesh.indexBuffer.Init(shape.indices.size() * sizeof(uint32_t),
                                 BufferUsage::INDEX_BUFFER | BufferUsage::TRANSFER_DESTINATION,
                                 MemoryUsage::GPU_ONLY);
        auto indexAllocation = stageBuffer.Submit(utils::MakeView(shape.indices));
        commandBuffer.CopyBuffer(BufferInfo{stageBuffer.GetBuffer(), indexAllocation.Offset},
                                 BufferInfo{submesh.indexBuffer, 0}, indexAllocation.Size);
        submesh.materialIndex = shape.materialIndex;
    }

    stageBuffer.Flush();
    commandBuffer.End();
    backend.SubmitCommandBuffer(commandBuffer);
    stageBuffer.Reset();

    uint32_t textureIndex = 0;
    for (const auto& material : model.materials) {
        commandBuffer.Begin();
        ImageLoader::FillImage(commandBuffer, mesh.textures.emplace_back(), material.albedoTexture,
                               ImageOptions::MIPMAPS);
        ImageLoader::FillImage(commandBuffer, mesh.textures.emplace_back(), material.normalTexture,
                               ImageOptions::MIPMAPS);
        ImageLoader::FillImage(commandBuffer, mesh.textures.emplace_back(),
                               material.metallicRoughness, ImageOptions::MIPMAPS);

        mesh.materials.push_back(
            gltf::GLTFMesh::Material{textureIndex, textureIndex + 1, textureIndex + 2, 1.0f, 1.0f});

        textureIndex += 3;
        stageBuffer.Flush();
        commandBuffer.End();
        backend.SubmitCommandBuffer(commandBuffer);
        stageBuffer.Reset();
    }

    m_gltfModel[std::string(resourceName)] = std::move(mesh);
}
} // namespace wind