#include "ForwardSceneRenderer.h"

namespace wind {
ForwardRenderer::ForwardRenderer() { Init(); }

void ForwardRenderer::Init() {
    const auto [width, height]    = RenderBackend::GetInstance().GetSurfaceExtent();
    const auto defaultColorFormat = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();

    for (const auto renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());

        TextureDesc backBufferDesc{width,
                                   height,
                                   defaultColorFormat,
                                   ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSFER_SOURCE,
                                   MemoryUsage::GPU_ONLY,
                                   ImageOptions::DEFAULT};

        TextureDesc depthBufferDesc{width,
                                    height,
                                    vk::Format::eD24UnormS8Uint,
                                    ImageUsage::DEPTH_SPENCIL_ATTACHMENT,
                                    MemoryUsage::GPU_ONLY,
                                    ImageOptions::DEFAULT};

        graphBuilder.AddRenderPass("OpaquePass", [&](PassNode* passNode) {
            passNode->DeclareColorAttachment("BackBuffer", backBufferDesc);
            passNode->DeclareDepthAttachment("OpaqueDepthBuffer", depthBufferDesc);

            passNode->CreateRenderPass();
            passNode->SetRenderRect(width, height);

            RenderProcessBuilder renderProcessBuilder;

            auto shader = ShaderFactory::CreateGraphicsShader("OpaqueShader", "Triangle.vert.spv",
                                                              "Triangle.frag.spv");

            renderProcessBuilder.SetBlendState(false)
                .SetShader(shader.get())
                .SetRenderPass(passNode->renderPass)
                .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

            passNode->pipelineState = renderProcessBuilder.BuildGraphicProcess();

            return [](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
                cmdBuffer.Draw(3, 1);
            };
        });

        graphBuilder.Compile();
    }
}

void ForwardRenderer::Render(Scene& scene) {
    m_backend.StartFrame();
    auto               currentImageIndex = m_backend.GetCurrentImageIndex();
    RenderGraphBuilder graphBuilder{m_renderGraphs[currentImageIndex].get()};
    graphBuilder.Exec();
    m_backend.EndFrame();
}

} // namespace wind