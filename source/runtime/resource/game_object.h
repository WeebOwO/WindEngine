#pragma once

#include "runtime/base/macro.h"
#include "runtime/resource/model.h"
#include <memory>

namespace wind {
class GameObject {
public:
    PERMIT_COPY(GameObject)

    GameObject(GameObject&& other)            = default;
    GameObject& operator=(GameObject&& other) = default;

    static GameObject createGameObject() {
        static uint32_t currentId = 0;
        return GameObject{currentId++};
    }

    auto getId() { return m_id; }

    std::shared_ptr<Model> model{};
    glm::vec3              color{};

private:
    GameObject(uint32_t id) : m_id(id) {}
    uint32_t m_id;
};
} // namespace wind