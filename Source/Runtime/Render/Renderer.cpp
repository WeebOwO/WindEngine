#include "Renderer.h"

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/Renderer.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    Init();
}

void Renderer::Init() {
    for(uint32_t i = 0; i < m_backend.GetMaxFrameInFlight(); ++i) {
        m_renderGraphs.push_back(std::make_shared<RenderGraph>());
    }
}

Renderer::~Renderer() {
    Quit();
}

void Renderer::Quit() {

}
} // namespace wind