#include "SceneView.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"
#include "Runtime/Resource/ImageLoader.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {

std::unordered_map<std::string, TextureDesc> SceneTexture::SceneTextureDescs = {
    {"SceneColor", TextureDesc{}}, {"SceneDepth", TextureDesc{}}, {"GBufferA", TextureDesc{}},
    {"GBufferB", TextureDesc{}},   {"GBufferC", TextureDesc{}},   {"GBufferD", TextureDesc{}}};

TextureDesc SceneView::sunShadowDesc{ShadowMapResolutionX,
                                     ShadowMapResolutionY,
                                     vk::SampleCountFlagBits::e1,
                                     vk::Format::eD16Unorm,
                                     ImageUsage::DEPTH_SPENCIL_ATTACHMENT | ImageUsage::SHADER_READ,
                                     MemoryUsage::GPU_ONLY,
                                     ImageOptions::DEFAULT};

static auto CreateImage = [](TextureDesc desc) {
    return std::make_shared<Image>(desc.width, desc.height, desc.format, desc.usage,
                                   desc.memoryUsage, desc.options);
};

void SceneView::SetScene(Scene* scene) {
    m_scene      = scene;
    auto& camera = m_scene->GetActiveCamera();
    // Update Camera Buffer and SkyBox buffer
    cameraBuffer->view     = camera->GetView();
    cameraBuffer->proj     = camera->GetProjection();
    cameraBuffer->viewproj = camera->GetProjection() * camera->GetView();
    cameraBuffer->proj[1][1] *= -1;
    cameraBuffer->cameraPos = camera->GetPosition();

    skyBoxBuffer->viewProj  = camera->GetProjection() * camera->GetView();
    skyBoxBuffer->cameraPos = camera->GetPosition();
    WIND_CORE_INFO("Current x is {}, y is {}, z is {}", cameraBuffer->cameraPos.x,
                   cameraBuffer->cameraPos.y, cameraBuffer->cameraPos.z);
    // Update lightBuffer
    auto sunData = scene->m_directionalLights.front();

    sunBuffer->lightPos   = sunData.lightPos;
    sunBuffer->direction  = sunData.direction;
    sunBuffer->ligthColor = sunData.ligthColor;
    sunBuffer->radiance   = sunData.radiance;

    // update light projection buffer

    glm::mat4 lightView = glm::lookAt(sunData.lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightProjection =
        glm::perspectiveFov(glm::radians(45.0f), (float)ShadowMapResolutionX,
                            (float)ShadowMapResolutionY, 1.0f, 10000.0f);
    lightProjection[1][1] *= -1;
    
    lightProjectionBuffer->lightProjection = lightProjection * lightView;

    skyBoxIrradianceTexture = scene->GetSkybox()->skyBoxIrradianceImage;
}

SceneView::SceneView() { Init(); }

void SceneView::Init() {
    cameraBuffer          = std::make_shared<CameraUnifoirmBuffer>();
    objectBuffer          = std::make_shared<ObjectUniformBuffer>();
    sunBuffer             = std::make_shared<SunUniformBuffer>();
    skyBoxBuffer          = std::make_shared<SkyBoxUniformBuffer>();
    lightProjectionBuffer = std::make_shared<LightProjectionBuffer>();

    // Load brdf lut
    iblBrdfLut = std::make_shared<Image>();
    ImageLoader::FillImage(*iblBrdfLut, Format::R8G8B8A8_SRGB,
                           R"(..\..\..\..\Assets\Textures\brdf_lut.dds)", ImageOptions::DEFAULT);
    // InitSceneTextureDesc
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

    SceneTexture::SceneTextureDescs["SceneColor"] = TextureDesc{
        width,
        height,
        vk::SampleCountFlagBits::e1,
        vk::Format::eR16G16B16A16Sfloat,
        ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSFER_SOURCE | ImageUsage::SHADER_READ,
        MemoryUsage::GPU_ONLY,
        ImageOptions::DEFAULT};

    SceneTexture::SceneTextureDescs["SceneDepth"] =
        TextureDesc{width,
                    height,
                    vk::SampleCountFlagBits::e1,
                    vk::Format::eD32SfloatS8Uint,
                    ImageUsage::DEPTH_SPENCIL_ATTACHMENT | ImageUsage::SHADER_READ,
                    MemoryUsage::GPU_ONLY,
                    ImageOptions::DEFAULT};
    // world position
    SceneTexture::SceneTextureDescs["GBufferA"] =
        TextureDesc{width,
                    height,
                    vk::SampleCountFlagBits::e1,
                    vk::Format::eR16G16B16A16Sfloat,
                    ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                    MemoryUsage::GPU_ONLY,
                    ImageOptions::DEFAULT};
    // world normal
    SceneTexture::SceneTextureDescs["GBufferB"] =
        TextureDesc{width,
                    height,
                    vk::SampleCountFlagBits::e1,
                    vk::Format::eR8G8B8A8Unorm,
                    ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                    MemoryUsage::GPU_ONLY,
                    ImageOptions::DEFAULT};
    // albedo
    SceneTexture::SceneTextureDescs["GBufferC"] =
        TextureDesc{width,
                    height,
                    vk::SampleCountFlagBits::e1,
                    vk::Format::eR8G8B8A8Unorm,
                    ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                    MemoryUsage::GPU_ONLY,
                    ImageOptions::DEFAULT};
    // m & r
    SceneTexture::SceneTextureDescs["GBufferD"] =
        TextureDesc{width,
                    height,
                    vk::SampleCountFlagBits::e1,
                    vk::Format::eR8G8B8A8Unorm,
                    ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                    MemoryUsage::GPU_ONLY,
                    ImageOptions::DEFAULT};

    // init shadow map
    sunShadowMap = CreateImage(sunShadowDesc);
}

SceneTexture SceneView::CreateSceneTextures(int createBit) {
    SceneTexture sceneTexture;

    auto& sceneTextureDesc  = SceneTexture::SceneTextureDescs;
    auto& sceneTexturesDict = sceneTexture.SceneTextures;

    if (createBit & SceneColor) {
        sceneTexture.sceneColor         = CreateImage(sceneTextureDesc["SceneColor"]);
        sceneTexturesDict["SceneColor"] = sceneTexture.sceneColor;
    }

    if (createBit & SceneDepth) {
        sceneTexture.sceneDepth         = CreateImage(sceneTextureDesc["SceneDepth"]);
        sceneTexturesDict["SceneDepth"] = sceneTexture.sceneDepth;
    }

    if (createBit & GBufferA) {
        sceneTexture.gbufferA         = CreateImage(sceneTextureDesc["GBufferA"]);
        sceneTexturesDict["GBufferA"] = sceneTexture.gbufferA;
    }

    if (createBit & GBufferB) {
        sceneTexture.gbufferB         = CreateImage(sceneTextureDesc["GBufferB"]);
        sceneTexturesDict["GBufferB"] = sceneTexture.gbufferB;
    }

    if (createBit & GBufferC) {
        sceneTexture.gbufferC         = CreateImage(sceneTextureDesc["GBufferC"]);
        sceneTexturesDict["GBufferC"] = sceneTexture.gbufferC;
    }

    if (createBit & GBufferD) {
        sceneTexture.gbufferD         = CreateImage(sceneTextureDesc["GBufferD"]);
        sceneTexturesDict["GBufferD"] = sceneTexture.gbufferD;
    }

    return sceneTexture;
}

} // namespace wind