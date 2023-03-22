#include "renderer.h"

#include <memory>

#include "GLFW/glfw3.h"

#include "runtime/render/context.h"

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window): m_context(window.GetWindow()) {}
private:
    RenderContext m_context;
    
};

Renderer::Renderer(Window& window): m_impl(std::make_unique<RenderImpl>(window)) {
    
}

Renderer::~Renderer() = default;

} // namespace wind