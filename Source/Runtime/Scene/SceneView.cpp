#include "SceneView.h"

namespace wind {
    SceneView::SceneView(Scene* scene) {
        m_scene = scene;
        auto& GameObjects = scene->GetWorldGameObjects();
    }
}