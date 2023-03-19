#pragma once

#include <memory>

#include "runtime/base/macro.h"
#include "runtime/render/context.h"

class GLFWwindow;

namespace wind {
class RenderImpl;

class Renderer {
public:
    Renderer(GLFWwindow* window);
    ~Renderer();
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
private:
    std::unique_ptr<RenderImpl> m_impl; // kind of rhi thing
};

} // namespace wind