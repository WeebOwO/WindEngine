#include "Scene.h"

namespace wind {
    void Scene::AddLightData(const DirectionalLight& directionalLight) {
        m_directionalLights.push_back(directionalLight);
    }

    void Scene::Init() {
        WIND_CORE_INFO("Scene is Init");
    }
}