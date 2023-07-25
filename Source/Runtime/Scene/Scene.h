#pragma once

#include <iostream>
#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Resource/GLTFLoader.h"
#include "Runtime/Resource/ImageData.h"
#include "Runtime/Resource/Mesh.h"
#include "Runtime/Scene/Camera.h"
#include "Runtime/Scene/GameObject.h"
#include "Runtime/Scene/Light.h"

namespace wind {
struct SkyBox {
    std::shared_ptr<Model> skyBoxModel;
    std::shared_ptr<Image> skyBoxImage;
    std::shared_ptr<Image> skyBoxIrradianceImage;
    SkyBox();
};

class Scene {
public:
    friend class SceneView;
    static void Init();

    static Scene& GetWorld() {
        static Scene world;
        return world;
    }

    void AddModel(const Model::Builder& modelBuilder) {
        auto model       = std::make_shared<Model>(modelBuilder);
        auto gameobject  = GameObject::CreateGameObject();
        gameobject.model = model;
        m_worldObjects.push_back(std::move(gameobject));
    }
    void AddLightData(const DirectionalLight& directionalLight);

    auto& GetWorldGameObjects() { return m_worldObjects; }
    auto& GetActiveCamera() { return m_activeCamera; }

    void SetupCamera(std::shared_ptr<BaseCamera> camera) { m_activeCamera = camera; }
    void LoadSkyBox(const std::string& skyBoxModelPath, const std::string& skyboxImagePath, const std::string& irradianceImagePath);
    void LoadGLTFScene(const std::string& resourceName, std::string_view filePath);

    auto& GetSkybox() { return m_skybox; }
    auto& GetRequiredGLTFModel(const std::string& resourname) {return m_gltfModel[resourname];}    

private:
    Scene() = default;
    std::vector<GameObject>       m_worldObjects;
    std::shared_ptr<BaseCamera>   m_activeCamera;
    std::vector<DirectionalLight> m_directionalLights;
    std::shared_ptr<SkyBox>       m_skybox; 
    // gltf part
    std::unordered_map<std::string, gltf::GLTFMesh> m_gltfModel;
};
} // namespace wind