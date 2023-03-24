#include "core.h"

#include "GLFW/glfw3.h"

#include <chrono>
#include <memory>

#include "runtime/render/context.h"
#include "runtime/render/renderer.h"
#include "runtime/render/window.h"

static constexpr uint32_t WIDTH  = 1920;
static constexpr uint32_t HEIGHT = 1080;

namespace wind {
// EngienImpl part
class EngineImpl {
public:
    EngineImpl(): m_window(WIDTH, HEIGHT, "Vulkan Engine") {   
        RenderContext::Init(m_window.GetWindow()); 
        m_renderer = std::make_unique<Renderer>(m_window);
    }

    ~EngineImpl() = default;

    void Run() {
        while (!glfwWindowShouldClose(m_window.GetWindow())) {    
            LogicTick();
            RenderTick();
        }
    }

private:
    void RenderTick();
    void LogicTick();
    Window m_window;
    std::unique_ptr<Renderer> m_renderer;
};

void EngineImpl::LogicTick() {
    glfwPollEvents();
}

void EngineImpl::RenderTick() {
    m_renderer->DrawFrame();
}

// Engine Part
void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {

}

Engine::~Engine() {
    RenderContext::GetInstace().device.waitIdle();
}

} // namespace wind