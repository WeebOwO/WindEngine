#include "ForwardSceneRenderer.h"

#include "Runtime/Render/PassRendering.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"

namespace wind {
ForwardRenderer::ForwardRenderer() { Init(); }

void ForwardRenderer::InitView(Scene& scene) { m_sceneView->SetScene(&scene); }

void ForwardRenderer::Init() {
    int createBits = SceneColor | SceneDepth;
    auto sceneTextures = m_sceneView->CreateSceneTextures(createBits);
    auto& backend = RenderBackend::GetInstance();
    // Call this to init every frame
    for (uint32_t index = 0; auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        graphBuilder.ImportSceneTextures(sceneTextures);
        auto swapchainImage = backend.AcquireSwapchainImage(index, ImageUsage::UNKNOWN);
        graphBuilder.ImportResource("BackBuffer", swapchainImage);
        // Add our renderpass
        AddSkyboxPass(graphBuilder);
        AddForwardBasePass(graphBuilder);
        AddBloomSetupPass(graphBuilder);
        AddToneMappingCombinePass(graphBuilder, index);
        graphBuilder.Compile();
        ++index;
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