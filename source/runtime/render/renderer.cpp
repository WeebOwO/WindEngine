#include "renderer.h"

#include <memory>

#include "GLFW/glfw3.h"
#include "runtime/base/macro.h"
#include "runtime/render/context.h"
#include "runtime/render/renderer.h"

namespace wind {
class RenderImpl {
public:
    RenderImpl(GLFWwindow* window): m_context(window) {}
private:
    RenderContext m_context;
};

Renderer::Renderer(GLFWwindow* window): m_impl(std::make_unique<RenderImpl>(window)) {
    
}

Renderer::~Renderer() = default;

} // namespace wind