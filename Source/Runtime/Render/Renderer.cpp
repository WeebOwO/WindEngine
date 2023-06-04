#include "Renderer.h"

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    m_renderGraph = std::make_unique<RenderGraph>();
    CreateRenderPass();
} 

void Renderer::CreateRenderPass() {
    m_renderGraph->AddRenderPass("BasePass", [&]() {
        WIND_CORE_INFO("Create Base Pass");
        return [](CommandBuffer cmdbuffer, RenderGraphRegister graphRegister) {
            
        };
    });
}

void Renderer::Render() {
    // Render start!
    // m_backend.StartFrame();
    // m_backend.EndFrame();
}
} // namespace wind