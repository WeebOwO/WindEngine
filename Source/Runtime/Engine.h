#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"

enum ShowCase : uint8_t {
    Pbr = 0, // Showcase for physical based rendering 
    Sponza, // todo: Showcase for tile based defer shading
    GPUDriven, // todo: Showcase for compute shader culling and indirectdraw
    SkinnedAnimation, // todo: Showcase for skin animation
    Physics,  // todo: This may not implement in short time
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