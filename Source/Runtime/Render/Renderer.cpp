#include "Renderer.h"

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/Renderer.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    m_renderGraph = std::make_unique<RenderGraph>();
}

void Renderer::Render() {
    // Render start!
    
    // m_backend.StartFrame();
    // m_backend.EndFrame();
}

void Renderer::RenderForward() {

}
} // namespace wind