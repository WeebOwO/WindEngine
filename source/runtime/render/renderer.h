#pragma once

#include <memory>

#include "runtime/base/macro.h"
#include "runtime/render/window.h"

class GLFWwindow;

namespace wind {
class RenderImpl;

class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    void DrawFrame();
    Renderer(Window& window);
    ~Renderer();

private:
    std::unique_ptr<RenderImpl> m_impl; // kind of rhi thing
};

} // namespace wind