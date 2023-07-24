#pragma once

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraph.h"
#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
enum class ShadingPath { Forward, Deferred };

class Scene;

class Renderer {
public:
    PERMIT_COPY(Renderer)
    PERMIT_MOVE(Renderer)
    Renderer();
    virtual ~Renderer();
    virtual void Render(Scene& scene) = 0;

protected:
    virtual void Init();
    virtual void Quit();
    virtual void RenderUI();
    virtual void InitView(Scene& scene) = 0;
    
protected:
    RenderBackend&                            m_backend;
    std::vector<std::shared_ptr<RenderGraph>> m_renderGraphs;
    std::shared_ptr<SceneView>                m_sceneView;
};

} // namespace wind