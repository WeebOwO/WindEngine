#include "Engine.h"

#include <memory>

#include "RUntime/Render/ForwardSceneRenderer.h"
#include "Runtime/Base/Log.h"
#include "Runtime/Input/Input.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Scene/Camera.h"
#include "Runtime/Scene/Scene.h"

static constexpr uint32_t WIDTH  = 800;
static constexpr uint32_t HEIGHT = 600;

namespace wind {
class EngineImpl {
public:
    EngineImpl() : m_window(WIDTH, HEIGHT, "Wind Engine") {
        Log::Init();
        WIND_CORE_INFO("Engine init");
        BackendCreateSetting setting{m_window};
        RenderBackend::Init(setting);
        Scene::Init();
        InputManger::Init(m_window.GetWindow());
        m_renderer = std::make_unique<ForwardRenderer>();
    }

    ~EngineImpl() = default;
    void Run() {
        LoadGameObject();
        AddCamera();
        while (!glfwWindowShouldClose(m_window.GetWindow())) {
            float fs = CalculateDeltaTime();
            LogicTick(fs);
            RenderTick(fs);
        }
    }

private:
    void  RenderTick(float ts);
    void  LogicTick(float ts);
    void  LoadGameObject();
    void  AddCamera();
    float CalculateDeltaTime();

private:
    Window                                m_window;
    std::unique_ptr<Renderer>             m_renderer{nullptr};
    std::chrono::steady_clock::time_point m_lastTickTimePoint{std::chrono::steady_clock::now()};
};

void  EngineImpl::AddCamera() {
    auto& world = Scene::GetWorld();
    world.SetupCamera(std::make_shared<Camera>(45.0f, 1.0f, 100.0f));

    vk::CommandBuffer cmdBuffer;
}

float EngineImpl::CalculateDeltaTime() {
    float dalta;
    {
        using namespace std::chrono;
        steady_clock::time_point tickTimePoint = steady_clock::now();
        auto          timeSpan =
            duration_cast<duration<float>>(tickTimePoint - m_lastTickTimePoint);
        dalta = timeSpan.count();

        m_lastTickTimePoint = tickTimePoint;
    }
    return dalta;
}
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
    v3.position = glm::vec3(-0.5, 0.5, 0.0);

    auto& world      = Scene::GetWorld();
    builder.vertices = {v1, v2, v3};
    world.AddModel(builder);
}

void EngineImpl::LogicTick(float fs) { 
    // window handle the flfw event
    auto& world = Scene::GetWorld();
    auto camera = world.GetActiveCamera();
    m_window.OnUpdate(fs);
    // update camera related things
    camera->OnResize(m_window.width(), m_window.height());
    camera->OnUpdate(fs);   
 }

void EngineImpl::RenderTick(float fs) { 
    auto& world = Scene::GetWorld();
    m_renderer->Render(world); 
}

// Engine Part
void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}

Engine::~Engine() { WIND_CORE_INFO("Engine shutdown"); }

} // namespace wind