#include "ForwardSceneRenderer.h"

#include <memory>

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
ForwardRenderer::ForwardRenderer() {
    Init();
}

void ForwardRenderer::AddForWardBasePass(RenderGraphBuilder& graphBuilder) {
    const auto defaultColorFormat = RenderBackend::GetInstance().GetSwapChainSurfaceFormat();
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

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
            "OpaqueShader", "ForwardBasePass.vert.spv", "ForwardBasePass.frag.spv");
        
        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = shader;
        passNode->pipelineState = renderProcessBuilder.BuildGraphicProcess();
        
        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto* scene = passNode->renderScene->GetOwnScene();
            
            for(auto& gameObject : scene->GetWorld().GetWorldGameObjects()) {
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
    auto& backend = RenderBackend::GetInstance();
    for(size_t i = 0; i < m_renderGraphs.size(); ++i) {
        RenderGraphBuilder graphBuilder(m_renderGraphs[i].get());
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