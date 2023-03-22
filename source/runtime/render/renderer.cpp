#include "runtime/render/renderer.h"

#include <memory>

#include "GLFW/glfw3.h"

#include "runtime/render/context.h"
#include "runtime/render/swapchain.h"

namespace wind {
class RenderImpl {
public:
    RenderImpl(Window& window): swapchain(window.width(), window.height()) {
        
    }
    
private:
   SwapChain swapchain;
};

Renderer::Renderer(Window& window): m_impl(std::make_unique<RenderImpl>(window)) {
    
}

Renderer::~Renderer() = default;

} // namespace wind