#include "Engine.h"

#include <memory>
#include <thread>

#include "RUntime/Render/ForwardSceneRenderer.h"
#include "Runtime/Base/Io.h"
#include "Runtime/Base/Log.h"
#include "Runtime/Input/Input.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/CommandBuffer.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Resource/ImageLoader.h"
#include "Runtime/Resource/Material.h"
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
    std::vector<std::thread>              m_threadPool;
};

void EngineImpl::AddCamera() {
    auto& world = Scene::GetWorld();
    world.SetupCamera(std::make_shared<OrbitCamera>(m_window.GetWindow()));
}

float EngineImpl::CalculateDeltaTime() {
    float dalta;
    {
        using namespace std::chrono;
        steady_clock::time_point tickTimePoint = steady_clock::now();
        auto timeSpan = duration_cast<duration<float>>(tickTimePoint - m_lastTickTimePoint);
        dalta         = timeSpan.count();

        m_lastTickTimePoint = tickTimePoint;
    }
    return dalta;
}

void EngineImpl::LoadGameObject() {
    auto&          world   = Scene::GetWorld();
    Model::Builder builder = io::LoadModelFromFilePath(R"(..\..\..\..\Assets\Mesh\cerberus.fbx)");
    Material       material;

    auto testCommandVecs = RenderBackend::GetInstance().RequestMultiCommandBuffer(4);

    m_threadPool.push_back(std::thread([&]() {
        ImageLoader::FillImage(*material.albedoTexture, CommandBuffer(testCommandVecs[0]),
                               R"(..\..\..\..\Assets\Textures\cerberus_A.png)",
                               ImageOptions::MIPMAPS);
    }));
    m_threadPool.push_back(std::thread([&]() {
        ImageLoader::FillImage(*material.metallicTexture, CommandBuffer(testCommandVecs[1]),
                               R"(..\..\..\..\Assets\Textures\cerberus_M.png)",
                               ImageOptions::MIPMAPS);
    }));
    m_threadPool.push_back(std::thread([&]() {
        ImageLoader::FillImage(*material.normalTexture, CommandBuffer(testCommandVecs[2]), 
                               R"(..\..\..\..\Assets\Textures\cerberus_N.png)",
                               ImageOptions::MIPMAPS);
    }));
    m_threadPool.push_back(std::thread([&]() {
        ImageLoader::FillImage(*material.roughnessTexture, CommandBuffer(testCommandVecs[3]),
                               R"(..\..\..\..\Assets\Textures\cerberus_R.png)",
                               ImageOptions::MIPMAPS);
    }));
    for (auto& t : m_threadPool) {
        t.join();
    }
    builder.material = material;
    world.AddModel(builder);
}

void EngineImpl::LogicTick(float fs) {
    // window handle the glfw event
    auto& world  = Scene::GetWorld();
    auto  camera = world.GetActiveCamera();
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