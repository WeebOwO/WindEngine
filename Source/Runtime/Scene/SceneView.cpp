#include "SceneView.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {

void SceneView::SetScene(Scene* scene) {
    m_scene      = scene;
    auto& camera = m_scene->GetActiveCamera();
    // Update Camera Buffer
    cameraBuffer->view     = camera->GetView();
    cameraBuffer->proj     = camera->GetProjection();
    cameraBuffer->viewproj = camera->GetProjection() * camera->GetView();
    cameraBuffer->proj[1][1] *= -1;

    // Update lightBuffer
    auto sun = scene->m_directionalLights.front();

    lightBuffer->lightDirection = sun.direction;
    lightBuffer->lightIntensity = sun.radiance;
    lightBuffer->lightColor     = sun.ligthColor;
}

SceneView::SceneView() { Init(); }

SceneView::SceneView(Scene* scene) : m_scene(scene) { Init(); }

void SceneView::Init() {
    cameraBuffer = std::make_shared<CameraUnifoirmBuffer>();
    objectBuffer = std::make_shared<ObjectUniformBuffer>();
    lightBuffer  = std::make_shared<LightUniformBuffer>();
}
} // namespace wind