#pragma once

#include <iostream>
#include <memory>


#include "runtime/base/macro.h"
#include "runtime/resource/game_object.h"
#include "runtime/resource/model.h"
#include "runtime/resource/vertex.h"


namespace wind {
class Scene {
public:
    static void   Init() { WIND_CORE_INFO("Scene Init!"); }
    static Scene& GetWorld() {
        static Scene world;
        return world;
    }

    void AddModel(const Model::Builder& modelBuilder) {
        auto model       = std::make_shared<Model>(modelBuilder);
        auto gameobject  = GameObject::createGameObject();
        gameobject.model = model;
        gameobject.color = {0.1f, 0.1f, 0.1f};
        m_WorldObjects.push_back(std::move(gameobject));
    }

    auto& GetWorldGameObjects() { return m_WorldObjects; }

private:
    std::vector<GameObject> m_WorldObjects;
};
} // namespace wind