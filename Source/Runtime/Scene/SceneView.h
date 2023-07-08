#pragma once

#include <memory>

#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
struct CameraUnifoirmBuffer {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 viewproj;
};

struct ObjectUniformBuffer {
    glm::mat4 model;
};

struct LightUniformBuffer {
    glm::vec3 lightDirection;
    glm::vec3 lightIntensity;
    glm::vec3 lightColor;
};

// A scene abstraction for renderer side data
class SceneView {
public:
    std::shared_ptr<CameraUnifoirmBuffer> cameraBuffer;
    std::shared_ptr<Image>                skybox;
    std::shared_ptr<ObjectUniformBuffer>  objectBuffer;
    std::shared_ptr<LightUniformBuffer>   lightBuffer;

    SceneView();
    SceneView(Scene* scene);
    void  Init();
    void  SetScene(Scene* scene);
    auto* GetOwnScene() { return m_scene; }

private:
    Scene* m_scene;
};
} // namespace wind