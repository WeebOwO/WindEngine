#include "DeferredSceneRenderer.h"

#include "Runtime/Render/PassRendering.h"

namespace wind {
DeferedSceneRenderer::DeferedSceneRenderer() { Init(); }

void DeferedSceneRenderer::Init() {
    int  createBits    = All;
    auto sceneTextures = m_sceneView->CreateSceneTextures(createBits);
    // Init render graph
    for (auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        graphBuilder.ImportSceneTextures(sceneTextures);
        graphBuilder.SetBackBufferName("SceneColor");
        // Just a triangle right now
        AddDeferedBasePass(graphBuilder);
        graphBuilder.Compile();
    }
}

void DeferedSceneRenderer::InitView(Scene& scene) { m_sceneView->SetScene(&scene); }

void DeferedSceneRenderer::Render(Scene& scene) {
    m_backend.StartFrame();
    InitView(scene);
    auto               currentImageIndex = m_backend.GetCurrentImageIndex();
    RenderGraphBuilder graphBuilder{m_renderGraphs[currentImageIndex].get()};
    graphBuilder.Setup(m_sceneView.get());
    graphBuilder.Exec();
    m_backend.EndFrame();
}

} // namespace wind