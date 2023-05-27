#include "Renderer.h"

#include <memory>

#include "Runtime/Render/BasePass.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    InitRenderPass();
    m_renderGraph.Setup();
} 

void Renderer::InitRenderPass() {
    m_renderGraph.AddRenderPass(std::make_unique<BasePass>("BasePass", m_renderGraph));
}

void Renderer::Render() {
    m_backend.StartFrame();
    m_renderGraph.Exec();
    m_backend.EndFrame();
}
} // namespace wind