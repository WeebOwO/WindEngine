#include "SceneView.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"
#include "Runtime/Resource/ImageLoader.h"

namespace wind {

std::unordered_map<std::string, TextureDesc> SceneTexture::SceneTextureDescs = {
    {"SceneColor", TextureDesc{}}, {"SceneDepth", TextureDesc{}}, {"GBufferA", TextureDesc{}},
    {"GBufferB", TextureDesc{}},   {"GBufferC", TextureDesc{}},   {"GBufferD", TextureDesc{}}};

void SceneView::SetScene(Scene* scene) {
    m_scene      = scene;
    auto& camera = m_scene->GetActiveCamera();
    // Update Camera Buffer and SkyBox buffer
    cameraBuffer->view     = camera->GetView();
    cameraBuffer->proj     = camera->GetProjection();
    cameraBuffer->viewproj = camera->GetProjection() * camera->GetView();
    cameraBuffer->proj[1][1] *= -1;
    cameraBuffer->cameraPos = camera->GetPosition();

    skyBoxBuffer->viewProj = camera->GetProjection() * camera->GetView();
    skyBoxBuffer->cameraPos = camera->GetPosition();
    // Update lightBuffer
    auto sun = scene->m_directionalLights.front();

    lightBuffer->lightDirection = sun.direction;
    lightBuffer->lightIntensity = sun.radiance;
    lightBuffer->lightColor     = sun.ligthColor;
    
    skyBoxIrradianceTexture = scene->GetSkybox()->skyBoxIrradianceImage;
}

SceneView::SceneView() { Init(); }

void SceneView::Init() {
    cameraBuffer = std::make_shared<CameraUnifoirmBuffer>();
    objectBuffer = std::make_shared<ObjectUniformBuffer>();
    lightBuffer  = std::make_shared<LightUniformBuffer>();
    skyBoxBuffer = std::make_shared<SkyBoxUniformBuffer>();
    // Load brdf lut
    iblBrdfLut = std::make_shared<Image>();
    ImageLoader::FillImage(*iblBrdfLut, Format::R8G8B8A8_SRGB, R"(..\..\..\..\Assets\Textures\brdf_lut.dds)", ImageOptions::DEFAULT);
    // InitSceneTextureDesc
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

    SceneTexture::SceneTextureDescs["SceneColor"] = TextureDesc{width,
                                                                height,
                                                                vk::SampleCountFlagBits::e1,
                                                                vk::Format::eR16G16B16A16Sfloat,
                                                                ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSFER_SOURCE | ImageUsage::SHADER_READ,
                                                                MemoryUsage::GPU_ONLY,
                                                                ImageOptions::DEFAULT};
    
    SceneTexture::SceneTextureDescs["SceneDepth"] =TextureDesc{width,
                                                            height,
                                                            vk::SampleCountFlagBits::e1,
                                                            vk::Format::eD32SfloatS8Uint,
                                                            ImageUsage::DEPTH_SPENCIL_ATTACHMENT | ImageUsage::SHADER_READ,
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