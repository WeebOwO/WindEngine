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

// A scene abstraction for renderer side data
class SceneView {
public:
    std::shared_ptr<CameraUnifoirmBuffer> cameraBuffer;

    SceneView();
    SceneView(Scene* scene);
    void  Init();
    void  SetScene(Scene* scene);
    auto* GetOwnScene() { return m_scene; }

private:
    Scene* m_scene;
};
} // namespace wind