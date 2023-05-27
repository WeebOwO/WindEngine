#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Runtime/Base/Macro.h"

namespace wind {

struct Transform {
   alignas(16) glm::vec3 location;
   alignas(16) glm::vec3 ratation;
   alignas(16) glm::vec3 scale;
};

class GameObject {
public:
    PERMIT_COPY(GameObject)

    GameObject(GameObject&& other)            = default;
    GameObject& operator=(GameObject&& other) = default;

    static GameObject CreateGameObject() {
        static uint32_t currentId = 0;
        return GameObject{currentId++};
    }

    auto GetId() { return m_id; }

    Transform transform;
    std::shared_ptr<Model> model{nullptr};

private:
    GameObject(uint32_t id) : m_id(id) {}
    uint32_t m_id;
};
} // namespace wind