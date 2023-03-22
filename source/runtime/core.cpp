#include "core.h"
#include "runtime/core.h"

#include <chrono>
#include <memory>

#include "runtime/render/window.h"
#include "runtime/render/renderer.h"

static constexpr uint32_t WIDTH  = 1920;
static constexpr uint32_t HEIGHT = 1080;

namespace wind {
class EngineImpl {
public:
    EngineImpl(): m_window(WIDTH, HEIGHT, "Vulkan Engine") {    
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
    Window m_window;
    std::unique_ptr<Renderer> m_renderer;
    void RenderTick() {}
    void LogicTick() { glfwPollEvents(); }
};

void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}
Engine::~Engine() = default;
} // namespace wind