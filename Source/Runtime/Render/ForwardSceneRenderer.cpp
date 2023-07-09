#include "ForwardSceneRenderer.h"

#include <memory>

#include "Runtime/Render/PassRendering.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
ForwardRenderer::ForwardRenderer() { Init(); }

void ForwardRenderer::InitView(Scene& scene) { m_sceneView->SetScene(&scene); }

void ForwardRenderer::Init() {
    int createBits = SceneColor | SceneDepth;
    for (auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        auto sceneTextures = m_sceneView->CreateSceneTextures(createBits);
        graphBuilder.ImportSceneTextures(sceneTextures);
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