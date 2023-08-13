#include "ParticleRenderer.h"
#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"

namespace wind {
void AddComputePass(const RenderGraphBuilder& graphbuilder) {}

void AddPresentPass(const RenderGraphBuilder& graphBuilder) {}

void ParticleRenderer::Render(Scene& scene) {
    m_backend.StartFrame();
    InitView(scene);
    auto               currentImageIndex = m_backend.GetCurrentImageIndex();
    RenderGraphBuilder graphBuilder{m_renderGraphs[currentImageIndex].get()};
    graphBuilder.Setup(m_sceneView.get());
    graphBuilder.Exec();
    m_backend.EndFrame();
}

void ParticleRenderer::Init() {
    auto& backend = RenderBackend::GetInstance();
    for (const auto index = 0; auto& renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        auto& swapchainImage = backend.AcquireSwapchainImage(index, ImageUsage::UNKNOWN);
        graphBuilder.ImportResource("BackBuffer", swapchainImage);
        AddComputePass(graphBuilder);
        AddPresentPass(graphBuilder);
    }
}

void ParticleRenderer::InitView(Scene& scene) { m_sceneView->SetScene(&scene); };
} // namespace wind