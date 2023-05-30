#include "Scene.h"

namespace wind {
    void Scene::Init() {
               
    }

    void Scene::UpdateUniformBuffer() {
        CameraViewData view;
        view.view = m_activeCamera->GetView();
        view.projection = m_activeCamera->GetProjection();
    }
}