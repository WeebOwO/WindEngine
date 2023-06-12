#include "Renderer.h"

#include <memory>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"

#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"
#include "Runtime/Render/Renderer.h"
#include "Runtime/Scene/Scene.h"

namespace wind {
Renderer::Renderer() : m_backend(RenderBackend::GetInstance()) {
    m_renderGraph = std::make_shared<RenderGraph>();
    if (m_shadingPath == ShadingPath::Forward) { InitForwardPassResource(); }
}

void Renderer::InitForwardPassResource() {
    RenderGraphBuilder graphBuilder(m_renderGraph.get());
    const auto [width, height]    = RenderBackend::GetInstance().GetSurfaceExtent();
    const auto defaultColorFormat = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();

    TextureDesc backBufferDesc{width,
                               height,
                               defaultColorFormat,
                               ImageUsage::COLOR_ATTACHMENT,
                               MemoryUsage::GPU_ONLY,
                               ImageOptions::DEFAULT};
    TextureDesc depthBufferDesc{width,
                                height,
                                vk::Format::eD24UnormS8Uint,
                                ImageUsage::DEPTH_SPENCIL_ATTACHMENT,
                                MemoryUsage::GPU_ONLY,
                                ImageOptions::DEFAULT};

    graphBuilder.AddRenderPass("OpaquePass", [&](PassNode& passNode) {
        passNode.DeclareColorAttachment("BackBuffer", backBufferDesc);
        passNode.DeclareDepthAttachment("OpaqueDepthBuffer", depthBufferDesc);

        passNode.CreateRenderPass();
        passNode.SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        renderProcessBuilder.SetBlendState(false)
            .SetRenderPass(passNode.renderPass)
            .SetShader("Triangle.vert.spv", "Triangle.frag.spv")
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode.pipelineState = renderProcessBuilder.BuildGraphicProcess();

        return [](CommandBuffer cmdBuffer, RenderGraphRegister graphRegister) {

        };
    });
}

void Renderer::Render(Scene& scene) {
    // m_backend.StartFrame();
    RenderGraphBuilder graphBuilder(m_renderGraph.get());
    RenderForward(graphBuilder);
    // m_backend.EndFrame();
}

void Renderer::RenderForward(RenderGraphBuilder& graphBuilder) {
    graphBuilder.Compile();
    graphBuilder.Exec();
}
} // namespace wind