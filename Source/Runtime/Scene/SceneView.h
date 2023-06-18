#pragma once

#include "Runtime/Resource/MeshBatch.h"
#include "Runtime/Scene/Scene.h"

#include <memory>

namespace wind {

struct ViewInfo {
    std::shared_ptr<Buffer> CameraUniformBuffer;
};

// A scene abstraction for renderer side data
class SceneView {
public:
    SceneView(Scene* scene);
    const auto* GetOwnScene() {return m_scene;}
private:
    Scene*   m_scene;
    ViewInfo m_viewInfo;
};
} // namespace wind