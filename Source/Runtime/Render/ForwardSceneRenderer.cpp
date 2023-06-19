#include "ForwardSceneRenderer.h"

#include <memory>

namespace wind {
ForwardRenderer::ForwardRenderer() {
    Init();
}

void AddForWardBasePass(RenderGraphBuilder& graphBuilder) {
    const auto defaultColorFormat = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();
    const auto [width, height]    = RenderBackend::GetInstance().GetSurfaceExtent();

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

    graphBuilder.AddRenderPass("OpaquePass", [=](PassNode* passNode) {
        passNode->DeclareColorAttachment("SceneColor", backBufferDesc);
        passNode->DeclareDepthAttachment("SceneDepth", depthBufferDesc);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> shader = ShaderFactory::CreateGraphicsShader(
            "OpaqueShader", "Triangle.vert.spv", "Triangle.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->pipelineState = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto* Scene = passNode->renderScene->GetOwnScene();
            for(auto& gameObject : Scene->GetWorld().GetWorldGameObjects()) {
                gameObject.model->Bind(cmdBuffer);
                gameObject.model->Draw(cmdBuffer);
            }
        };
    });
}

void ForwardRenderer::InitView(Scene& scene) {
    m_sceneView->SetScene(&scene);
}

void ForwardRenderer::Init() {
    for (auto renderGraph : m_renderGraphs) {
        RenderGraphBuilder graphBuilder(renderGraph.get());
        graphBuilder.SetBackBufferName("SceneColor");
        AddForWardBasePass(graphBuilder);
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