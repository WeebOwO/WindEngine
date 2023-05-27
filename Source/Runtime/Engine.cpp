#include "Engine.h"

#include <memory>

#include "Runtime/Base/Log.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Scene/Scene.h"
#include "RUntime/Render/Renderer.h"

static constexpr uint32_t WIDTH  = 1920;
static constexpr uint32_t HEIGHT = 1080;

namespace wind {
class EngineImpl {
public:
    EngineImpl() : m_window(WIDTH, HEIGHT, "Wind Engine") {
        Log::Init();
        WIND_CORE_INFO("Engine init");
        BackendCreateSetting setting{m_window};
        RenderBackend::Init(setting);
        m_renderer = std::make_unique<Renderer>();
    }

    ~EngineImpl() = default;
    void Run() {
        LoadGameObject();
        while (!glfwWindowShouldClose(m_window.GetWindow())) {
            LogicTick();
            RenderTick();
        }
    }

private:
    void                      RenderTick();
    void                      LogicTick();
    void                      LoadGameObject();
    Window                    m_window;
    std::unique_ptr<Renderer> m_renderer{nullptr};
};

void EngineImpl::LoadGameObject() {
    // Model::Builder builder;    
    // auto& world = Scene::GetWorld();
    // builder = io::LoadModelFromFilePath("../../../../assets/meshes/skybox.obj");
    // world.AddModel(builder);

    Model::Builder builder;
    builder.indices = {0, 1, 2};
    
    Vertex v1, v2, v3;
    v1.position = glm::vec3(0.0, -0.5, 0.0);
    v2.position = glm::vec3(0.5, 0.5, 0.0);
    v3.position = glm::vec3(0.5, -0.5, 0.0);
    
    auto& world = Scene::GetWorld();
    builder.vertices = {v1, v2, v3};
    world.AddModel(builder);
}

void EngineImpl::LogicTick() { 
    m_window.OnUpdate();    
}

void EngineImpl::RenderTick() { 
    m_renderer->Render(); 
}


// Engine Part
void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}

Engine::~Engine() { WIND_CORE_INFO("Engine shutdown"); }

} // namespace wind