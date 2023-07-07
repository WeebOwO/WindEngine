#pragma once

#include <iostream>
#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Resource/ImageData.h"
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

    void SetupCamera(std::shared_ptr<BaseCamera> camera) { m_activeCamera = camera; }

    void BuildMeshBatch();

private:
    Scene();
    std::vector<GameObject>     m_worldObjects;
    std::shared_ptr<BaseCamera> m_activeCamera;
    ImageData                   m_SkyBoxData;
};
} // namespace wind