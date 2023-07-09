#include "SceneView.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

namespace wind {

std::unordered_map<std::string, TextureDesc> SceneTexture::SceneTextureDescs = {
    {"SceneColor", TextureDesc{}}, {"SceneDepth", TextureDesc{}}, {"GBufferA", TextureDesc{}},
    {"GBufferB", TextureDesc{}},   {"GBufferC", TextureDesc{}},   {"GBufferD", TextureDesc{}}};

void SceneView::SetScene(Scene* scene) {
    m_scene      = scene;
    auto& camera = m_scene->GetActiveCamera();
    // Update Camera Buffer
    cameraBuffer->view     = camera->GetView();
    cameraBuffer->proj     = camera->GetProjection();
    cameraBuffer->viewproj = camera->GetProjection() * camera->GetView();
    cameraBuffer->proj[1][1] *= -1;

    // Update lightBuffer
    auto sun = scene->m_directionalLights.front();

    lightBuffer->lightDirection = sun.direction;
    lightBuffer->lightIntensity = sun.radiance;
    lightBuffer->lightColor     = sun.ligthColor;
}

SceneView::SceneView() { Init(); }

void SceneView::Init() {
    cameraBuffer = std::make_shared<CameraUnifoirmBuffer>();
    objectBuffer = std::make_shared<ObjectUniformBuffer>();
    lightBuffer  = std::make_shared<LightUniformBuffer>();

    // InitSceneTextureDesc
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

    SceneTexture::SceneTextureDescs["SceneColor"] = TextureDesc{width,
                                                                height,
                                                                vk::Format::eB8G8R8A8Unorm,
                                                                ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSFER_SOURCE,
                                                                MemoryUsage::GPU_ONLY,
                                                                ImageOptions::DEFAULT};

    SceneTexture::SceneTextureDescs["SceneDepth"] =TextureDesc{width,
                                                            height,
                                                            vk::Format::eD24UnormS8Uint,
                                                            ImageUsage::DEPTH_SPENCIL_ATTACHMENT,
                                                            MemoryUsage::GPU_ONLY,
                                                            ImageOptions::DEFAULT};
    
}

SceneTexture SceneView::CreateSceneTextures(int createBit) {
    auto CreateImage = [&](TextureDesc desc) {
        return std::make_shared<Image>(desc.width, desc.height, desc.format, desc.usage, desc.memoryUsage, desc.options); 
    };
    SceneTexture sceneTexture;

    auto& sceneTextureDesc = SceneTexture::SceneTextureDescs;
    auto& sceneTexturesDict = sceneTexture.SceneTextures;
    if(createBit & SceneColor) {
        sceneTexture.sceneColor = CreateImage(sceneTextureDesc["SceneColor"]);
        sceneTexturesDict["SceneColor"] = sceneTexture.sceneColor;
    } 
    if(createBit & SceneDepth) {
        sceneTexture.sceneDepth = CreateImage(sceneTextureDesc["SceneDepth"]);
        sceneTexturesDict["SceneDepth"] = sceneTexture.sceneDepth;
    }
    return sceneTexture;
}
} // namespace wind