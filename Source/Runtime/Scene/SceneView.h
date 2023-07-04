#pragma once

#include <memory>

#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
struct CameraUnifoirmBuffer {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
};

struct ObjectUniformBuffer {
    glm::mat4 model;
};

// A scene abstraction for renderer side data
class SceneView {
public:
    std::shared_ptr<CameraUnifoirmBuffer> cameraBuffer;
    
    SceneView() = default;
    SceneView(Scene* scene);
    void  SetScene(Scene* scene);
    auto* GetOwnScene() { return m_scene; }

private:
    Scene* m_scene;
    CameraUnifoirmBuffer m_cameraUniformBuffer;
};
} // namespace wind