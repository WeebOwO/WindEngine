#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraph.h"
#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"

namespace wind {
enum class MeshPassType { BasePass = 0 };

enum class ShadingPath { Forward, Deferred };

class Scene;

class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    Renderer();
    virtual ~Renderer();
    virtual void Render(Scene& scene) = 0;
    virtual void Init();
    virtual void Quit();

protected:
    RenderBackend&                            m_backend;
    std::vector<std::shared_ptr<RenderGraph>> m_renderGraphs;
};

} // namespace wind