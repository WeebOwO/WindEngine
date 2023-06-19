#pragma once

#include "Runtime/Resource/MeshBatch.h"
#include "Runtime/Scene/Scene.h"

#include <memory>

namespace wind {

class CameraUniformBuffer {
public:
    void InitCameraBuffer();
private:
    std::shared_ptr<Buffer> m_rhiBuffer;
};

struct ViewInfo {

};

// A scene abstraction for renderer side data
class SceneView {
public:
    SceneView() = default;
    SceneView(Scene* scene);
    void SetScene(Scene* scene) {m_scene = scene;}
    const auto* GetOwnScene() {return m_scene;}
private:
    Scene*   m_scene;
    ViewInfo m_viewInfo;
};
} // namespace wind