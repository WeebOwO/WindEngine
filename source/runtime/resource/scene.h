#pragma once

#include <memory>
#include <iostream>

#include "runtime/base/macro.h"
#include "runtime/resource/model.h"
#include "runtime/resource/vertex.h"
#include "runtime/resource/game_object.h"

namespace wind {
class Scene {
public:
    static void Init() {
        WIND_CORE_INFO("Scene Init!");
    }
    static Scene& GetWorld() {
        static Scene world;
        return world;
    }

    void AddModel(const std::vector<Vertex>& vertices) {
        auto model = std::make_shared<Model>(vertices);
        auto gameobject = GameObject::createGameObject();
        gameobject.model = model;
        gameobject.color = {0.1f, 0.1f, 0.1f};
        m_WorldObjects.push_back(std::move(gameobject));
    }

    auto& GetWorldGameObjects() {return m_WorldObjects;}
private:
    std::vector<GameObject> m_WorldObjects;
};
} // namespace wind