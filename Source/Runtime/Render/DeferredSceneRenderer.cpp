#include "DeferredSceneRenderer.h"

#include "Runtime/Render/PassRendering.h"
#include "Runtime/Render/RHI/Backend.h"

namespace wind {
DeferedSceneRenderer::DeferedSceneRenderer() { Init(); }

void DeferedSceneRenderer::Init() {
    int  createBits    = All;
    auto sceneTextures = m_sceneView->CreateSceneTextures(createBits);
    auto& backend = RenderBackend::GetInstance();
    // Init render graph
    for (uint32_t index = 0; auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        graphBuilder.ImportSceneTextures(sceneTextures);
        auto& swapchainImage = backend.AcquireSwapchainImage(index, ImageUsage::UNKNOWN);
        graphBuilder.ImportResource("BackBuffer", swapchainImage);
        // Add our renderpass
        AddDeferedBasePass(graphBuilder);
        AddDeferToneMappingCombinePass(graphBuilder);
        graphBuilder.Compile();
        ++index;
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