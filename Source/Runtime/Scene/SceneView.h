#pragma once

#include <memory>
#include <unordered_map>

#include "Runtime/Render/RenderGraph/RenderResource.h"

#include "Runtime/Scene/GPUScene.h"
#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Scene.h"


namespace wind {
struct CameraUnifoirmBuffer {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 viewproj;
    glm::vec3 cameraPos;
};

struct SkyBoxUniformBuffer {
    glm::mat4 viewProj;
    glm::vec3 cameraPos;
};

struct ObjectUniformBuffer {
    glm::mat4 model;
};

struct LightUniformBuffer {
    glm::vec3 lightDirection;
    glm::vec3 lightIntensity;
    glm::vec3 lightColor;
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

// A scene abstraction for renderer side data
class SceneView {
public:
    std::shared_ptr<CameraUnifoirmBuffer> cameraBuffer;
    std::shared_ptr<Image>                skybox;
    std::shared_ptr<ObjectUniformBuffer>  objectBuffer;
    std::shared_ptr<LightUniformBuffer>   lightBuffer;
    std::shared_ptr<SkyBoxUniformBuffer>  skyBoxBuffer;

    // For ibl calc
    std::shared_ptr<Image> iblBrdfLut;
    std::shared_ptr<Image> skyBoxIrradianceTexture;

    SceneView();
    void Init();
    void SetScene(Scene* scene);
    
    auto*        GetOwnScene() { return m_scene; }
    SceneTexture CreateSceneTextures(int createBit);

private:
    Scene* m_scene;
};
} // namespace wind