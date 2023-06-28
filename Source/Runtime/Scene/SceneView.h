#pragma once

#include <memory>

#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
// A scene abstraction for renderer side data
class SceneView {
public:
    SceneView() = default;
    SceneView(Scene* scene);
    void  SetScene(Scene* scene);
    auto* GetOwnScene() { return m_scene; }
  
private:
    Scene*   m_scene;
};
} // namespace wind