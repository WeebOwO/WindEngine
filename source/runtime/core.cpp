#include "core.h"

#include "GLFW/glfw3.h"

#include <chrono>
#include <memory>

#include "runtime/base/utils.h"

#include "runtime/render/context.h"
#include "runtime/render/renderer.h"
#include "runtime/render/window.h"
#include "runtime/resource/model.h"
#include "runtime/resource/scene.h"

static constexpr uint32_t WIDTH  = 1920;
static constexpr uint32_t HEIGHT = 1080;

namespace wind {
// EngienImpl part
class EngineImpl {
public:
    EngineImpl() : m_window(WIDTH, HEIGHT, "Vulkan Engine") {
        Log::Init();
        RenderContext::Init(m_window.GetWindow());
        Scene::Init();
        m_renderer = std::make_unique<Renderer>(m_window);
    }

    ~EngineImpl() = default;

    void Run() {
        LoadGameObjects();
        while (!glfwWindowShouldClose(m_window.GetWindow())) {
            LogicTick();
            RenderTick();
        }
    }

private:
    void                      RenderTick();
    void                      LogicTick();
    void                      LoadGameObjects() const noexcept;
    Window                    m_window;
    std::unique_ptr<Renderer> m_renderer;
};

void EngineImpl::LogicTick() { glfwPollEvents(); }

void EngineImpl::RenderTick() { m_renderer->DrawFrame(); }

void EngineImpl::LoadGameObjects() const noexcept {
    Model::Builder            builder;
    const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    builder.vertices = vertices;
    builder.indices  = {0, 1, 2, 2, 3, 0};
    auto& world      = Scene::GetWorld();
    world.AddModel(builder);
}

// Engine Part
void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}

Engine::~Engine() {
    auto& device = utils::GetRHIDevice();
    device.waitIdle();
}

} // namespace wind