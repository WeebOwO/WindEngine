#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraph.h"

namespace wind {

enum class MeshPassType {
    BasePass = 0
};

enum class ShadingPath  {
    Forward, 
    Deferred
};

class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    Renderer();
    void CreateRenderPass();
    void Render();
    
private:
    void RenderForward();
    void RenderDefered();
    ShadingPath m_shadingPath {ShadingPath::Forward};
    RenderBackend& m_backend;
    std::unique_ptr<RenderGraph> m_renderGraph;
};
} // namespace wind