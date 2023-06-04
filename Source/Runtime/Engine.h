#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"

namespace wind {
class EngineImpl;

class Engine {
public:
    PERMIT_COPY(Engine)
    PERMIT_MOVE(Engine)
    Engine();
    ~Engine();
    void Run();

private:
    std::unique_ptr<EngineImpl> m_impl;
};
}; // namespace wind