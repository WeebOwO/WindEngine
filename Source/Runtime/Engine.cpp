#include "Engine.h"

#include <memory>
#include <thread>

#include "Runtime/Base/Io.h"
#include "Runtime/Base/Log.h"
#include "Runtime/Input/Input.h"
#include "RUntime/Render/ForwardSceneRenderer.h"
#include "Runtime/Render/DeferredSceneRenderer.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Resource/ImageLoader.h"
#include "Runtime/Resource/Material.h"
#include "Runtime/Resource/GLTFLoader.h"
#include "Runtime/Scene/Camera.h"
#include "Runtime/Scene/Scene.h"

static constexpr uint32_t WIDTH  = 1080;
static constexpr uint32_t HEIGHT = 720;

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
        InitScene();
    }

    ~EngineImpl() = default;
    void Run() {
        LoadGameObject();
        while (!glfwWindowShouldClose(m_window.GetWindow())) {
            float fs = CalculateDeltaTime();
            LogicTick(fs);
            RenderTick(fs);
        }
    }

    void SetShowCase(ShowCase showcase) {
        m_showCase = showcase;
        if (showcase == ShowCase::Pbr) { 
            m_renderer = std::make_unique<ForwardRenderer>(); 
        }
        if(showcase == ShowCase::Sponza) {
            m_renderer = std::make_unique<DeferedSceneRenderer>();
        }
    }

private:
    void  RenderTick(float ts);
    void  LogicTick(float ts);
    void  LoadGameObject();
    void  InitScene();
    void  AddCamera();
    float CalculateDeltaTime();

private:
    Window                                m_window;
    std::unique_ptr<Renderer>             m_renderer{nullptr};
    std::chrono::steady_clock::time_point m_lastTickTimePoint{std::chrono::steady_clock::now()};
    std::vector<std::thread>              m_threadPool;
    ShowCase                              m_showCase{ShowCase::Pbr};
};

void EngineImpl::AddCamera() {
    auto& world = Scene::GetWorld();
    if (m_showCase == ShowCase::Pbr) {
        world.SetupCamera(std::make_shared<OrbitCamera>(m_window.GetWindow()));
    } else {
        world.SetupCamera(std::make_shared<FirstPersonCamera>(45.0f, 0.1f, 100.0f));
    }
}

void EngineImpl::InitScene() {
    auto& world = Scene::GetWorld();
    world.SetupCamera(std::make_shared<OrbitCamera>(m_window.GetWindow()));

    DirectionalLight sun;
    sun.direction = glm::normalize(glm::vec3{-1.0f, 0.0f, 0.0f});
    sun.radiance = glm::vec3{1.0f, 1.0f, 1.0f};

    world.AddLightData(sun);

    world.LoadSkyBox(R"(..\..\..\..\Assets\Mesh\skybox.obj)",
                     R"(..\..\..\..\Assets\Textures\skybox.png)",
                     R"(..\..\..\..\Assets\Textures\skybox_irradiance.png)");
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
    auto& world = Scene::GetWorld();
    switch (m_showCase) {
    case ShowCase::Pbr: {
        // todo: current is hard code our model asset, try get it better
        Model::Builder builder =
            io::LoadModelFromFilePath(R"(..\..\..\..\Assets\Mesh\cerberus.fbx)");
        Material material;

        ImageData albeoData, normalData, metallicdata, roughnessData;

        m_threadPool.push_back(std::thread([&]() {
            albeoData = ImageLoader::LoadImageDataFromFile(
                R"(..\..\..\..\Assets\Textures\cerberus_A.png)", Format::R8G8B8A8_SRGB);
        }));

        m_threadPool.push_back(std::thread([&]() {
            normalData = ImageLoader::LoadImageDataFromFile(
                R"(..\..\..\..\Assets\Textures\cerberus_N.png)", Format::R8G8B8A8_UNORM);
        }));

        m_threadPool.push_back(std::thread([&]() {
            metallicdata = ImageLoader::LoadImageDataFromFile(
                R"(..\..\..\..\Assets\Textures\cerberus_M.png)", Format::R8_UNORM);
        }));

        m_threadPool.push_back(std::thread([&]() {
            roughnessData = ImageLoader::LoadImageDataFromFile(
                R"(..\..\..\..\Assets\Textures\cerberus_R.png)", Format::R8_UNORM);
        }));

        for (auto& t : m_threadPool) {
            t.join();
        }

        ImageLoader::FillImage(*material.albedoTexture, albeoData, ImageOptions::MIPMAPS);
        ImageLoader::FillImage(*material.normalTexture, normalData, ImageOptions::MIPMAPS);
        ImageLoader::FillImage(*material.metallicTexture, metallicdata, ImageOptions::MIPMAPS);
        ImageLoader::FillImage(*material.roughnessTexture, roughnessData, ImageOptions::MIPMAPS);

        builder.material = material;
        world.AddModel(builder);
        break;
    } 
    case ShowCase::Sponza : {
        // just draw triangle right now
        
        
        break;
    }
    default:
        break;
    }
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

void Engine::SetShowCase(ShowCase showcase) { m_impl->SetShowCase(showcase); }
} // namespace wind