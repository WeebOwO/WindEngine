#pragma once

#include <memory>
#include <unordered_map>

#include "Runtime/Render/RenderGraph/RenderResource.h"

#include "Runtime/Scene/GPUScene.h"
#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Light.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
struct CameraUnifoirmBuffer {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 viewproj;
    alignas(16) glm::vec3 cameraPos;
};

struct SkyBoxUniformBuffer {
    glm::mat4 viewProj;
    glm::vec3 cameraPos;
};

struct ObjectUniformBuffer {
    glm::mat4 model;
};

enum SceneTextureCreateBit : uint32_t {
    SceneColor = BIT(0),
    SceneDepth = BIT(1),
    GBufferA   = BIT(2),
    GBufferB   = BIT(3),
    GBufferC   = BIT(4),
    GBufferD   = BIT(5),
    All        = SceneColor | SceneDepth | GBufferA | GBufferB | GBufferC | GBufferD
};

struct SunUniformBuffer {
    alignas(16) glm::vec3 lightPos{};
    alignas(16) glm::vec3 direction{};
    alignas(16) glm::vec3 radiance{1.0f};
    alignas(16) glm::vec3 ligthColor{1.0f};
};

class SceneTexture {
public:
    static std::unordered_map<std::string, TextureDesc>     SceneTextureDescs;
    std::unordered_map<std::string, std::shared_ptr<Image>> SceneTextures;

    std::shared_ptr<Image> sceneColor;
    std::shared_ptr<Image> sceneDepth;
    std::shared_ptr<Image> gbufferA;
    std::shared_ptr<Image> gbufferB;
    std::shared_ptr<Image> gbufferC;
    std::shared_ptr<Image> gbufferD;
};

struct LightProjectionBuffer {
    glm::mat4 lightProjection;
};

// A scene abstraction for renderer side data
class SceneView {
public:
    constexpr static uint32_t ShadowMapResolutionX = 2048;
    constexpr static uint32_t ShadowMapResolutionY = 2048;

    std::shared_ptr<CameraUnifoirmBuffer>  cameraBuffer;
    std::shared_ptr<Image>                 skybox;
    std::shared_ptr<ObjectUniformBuffer>   objectBuffer;
    std::shared_ptr<SunUniformBuffer>      sunBuffer;
    std::shared_ptr<SkyBoxUniformBuffer>   skyBoxBuffer;
    std::shared_ptr<LightProjectionBuffer> lightProjectionBuffer;

    // For ibl calc
    std::shared_ptr<Image> iblBrdfLut;
    std::shared_ptr<Image> skyBoxIrradianceTexture;

    // shadow map and light projection
    static TextureDesc sunShadowDesc;
    std::shared_ptr<Image> sunShadowMap;

    SceneView();
    void Init();
    void SetScene(Scene* scene);

    auto*        GetOwnScene() { return m_scene; }
    SceneTexture CreateSceneTextures(int createBit);

private:
    Scene* m_scene;
};
} // namespace wind