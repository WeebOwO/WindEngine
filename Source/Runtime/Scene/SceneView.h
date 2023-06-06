#pragma once

#include "Runtime/Scene/Scene.h"

#include <memory>

namespace wind {
struct ViewInfo {};
// A scene abstraction for renderer side
class SceneView {
public:
    SceneView(Scene* scene) : m_scene(scene) {}
private:
    Scene*   m_scene;
    ViewInfo m_viewInfo;
};
} // namespace wind