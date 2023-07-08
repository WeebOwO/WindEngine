#include "ForwardSceneRenderer.h"

#include <memory>

#include "Runtime/Render/PassRendering.h"

namespace wind {
ForwardRenderer::ForwardRenderer() { Init(); }

void ForwardRenderer::InitView(Scene& scene) { m_sceneView->SetScene(&scene); }

void ForwardRenderer::Init() {
    for (auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        graphBuilder.SetBackBufferName("SceneColor");
        AddForwardBasePass(graphBuilder);
        graphBuilder.Compile();
    }
}

void ForwardRenderer::Render(Scene& scene) {
    m_backend.StartFrame();
    InitView(scene);
    auto               currentImageIndex = m_backend.GetCurrentImageIndex();
    RenderGraphBuilder graphBuilder{m_renderGraphs[currentImageIndex].get()};
    graphBuilder.Setup(m_sceneView.get());
    graphBuilder.Exec();
    m_backend.EndFrame();
}
} // namespace wind