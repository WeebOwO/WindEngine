#pragma once

#include <iostream>
#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Resource/Mesh.h"
#include "Runtime/Scene/Camera.h"
#include "Runtime/Scene/GameObject.h"


namespace wind {
class Scene {
public:
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

    auto& GetWorldGameObjects() { return m_worldObjects; }
    auto& GetActiveCamera() { return m_activeCamera; }

    auto GetViewUniformBuffer() {return m_viewUniformBuffer;}
    auto GetSceneUniformBuffer();

    void UpdateUniformBuffer();
    void SetupCamera(std::shared_ptr<Camera> camera) {
        m_activeCamera = camera;
    }

private:
    struct CameraViewData {
        glm::mat4 view;
        glm::mat4 projection;
    };

    std::vector<GameObject> m_worldObjects;
    std::shared_ptr<Buffer> m_viewUniformBuffer;
    std::shared_ptr<Buffer> m_sceneUniforBuffer;
    std::shared_ptr<Camera> m_activeCamera;
};
} // namespace wind