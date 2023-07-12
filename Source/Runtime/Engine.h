#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"

enum ShowCase : uint8_t {
    Pbr = 0, // Showcase for physical based rendering 
    Sponza = 1 // todo: Showcase for tile based defer shading
};

namespace wind {
class EngineImpl;

class Engine {
public:
    PERMIT_COPY(Engine)
    PERMIT_MOVE(Engine)
    Engine();
    ~Engine();
    void Run();
    void SetShowCase(ShowCase showcase);
private:
    std::unique_ptr<EngineImpl> m_impl;
};
}; // namespace wind