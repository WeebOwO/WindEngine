#include "Scene.h"

#include "Runtime/Base/Io.h"
#include "Runtime/Resource/ImageLoader.h"
#include "Runtime/Scene/Scene.h"
#include <memory>

namespace wind {
    SkyBox::SkyBox() {
        skyBoxImage = std::make_shared<Image>();
        skyBoxIrradianceImage = std::make_shared<Image>();
    }
    void Scene::AddLightData(const DirectionalLight& directionalLight) {
        m_directionalLights.push_back(directionalLight);
    }

    void Scene::Init() {
        WIND_CORE_INFO("Scene is Init");
    }

    void Scene::LoadSkyBox(const std::string &skyBoxModelPath, const std::string& skyboxImagePath, const std::string& irradianceImagePath) {
        m_skybox = std::make_shared<SkyBox>();
        Model::Builder builder = io::LoadModelFromFilePath(skyBoxModelPath);
        ImageLoader::LoadCubemap(*m_skybox->skyBoxImage, Format::R8G8B8A8_UNORM, skyboxImagePath);
        m_skybox->skyBoxModel = std::make_shared<Model>(builder);
        ImageLoader::LoadCubemap(*m_skybox->skyBoxIrradianceImage, Format::R8G8B8A8_UNORM, skyboxImagePath);
    }
}