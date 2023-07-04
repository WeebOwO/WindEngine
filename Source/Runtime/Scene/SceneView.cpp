#include "SceneView.h"

#include "Runtime/Base/Macro.h"
#include "Runtime/Scene/SceneView.h"
#include "Runtime/Render/RHI/Buffer.h"

namespace wind {

void SceneView::SetScene(Scene* scene) { 
    m_scene = scene;
    auto& camera = m_scene->GetActiveCamera();
    // Update Camera Buffer 
    m_cameraUniformBuffer.view = camera->GetView();
    m_cameraUniformBuffer.proj = camera->GetProjection();
    m_cameraUniformBuffer.viewproj = camera->GetView() * camera->GetProjection();
}

SceneView::SceneView(Scene* scene) : m_scene(scene) {
    
}
} // namespace wind