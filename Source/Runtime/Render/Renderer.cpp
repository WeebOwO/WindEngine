#include "Renderer.h"

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    Init();
}

void Renderer::Init() {
    m_sceneView = std::make_unique<SceneView>();
    for(uint32_t i = 0; i < m_backend.GetMaxFrameInFlight(); ++i) {
        m_renderGraphs.push_back(std::make_shared<RenderGraph>());
    }
}

Renderer::~Renderer() {
    Quit();
}

void Renderer::Quit() {
    WIND_CORE_INFO("Quit renderer");
}

void Renderer::RenderUI() {
    return;
}
} // namespace wind