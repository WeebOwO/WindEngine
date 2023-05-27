#pragma once

#include <iostream>
#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Resource/Mesh.h"
#include "Runtime/Scene/GameObject.h"

namespace wind {
class Scene {
public:
    static void Init() { WIND_CORE_INFO("Scene Init!"); }
    
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
    
private:
    std::vector<GameObject> m_worldObjects;
};
} // namespace wind