#include "Renderer.h"

#include <memory>

#include "Runtime/Render/BasePass.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    InitRenderPass();
    m_renderGraph.Setup();
} 

void Renderer::InitRenderPass() {
    // Currently just simple forward shading
    m_renderGraph.AddRenderPass(std::make_unique<BasePass>("BasePass", m_renderGraph));
}

void Renderer::Render() {
    auto& world = Scene::GetWorld();
    world.UpdateUniformBuffer();
    // Render start!
    m_backend.StartFrame();
    m_renderGraph.Exec();
    m_backend.EndFrame();
}
} // namespace wind