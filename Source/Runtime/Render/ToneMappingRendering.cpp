#include "PassRendering.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

namespace wind {
void AddToneMappingCombinePass(RenderGraphBuilder& graphBuilder, uint32_t swapChainImageIndex) {
    auto& backend = RenderBackend::GetInstance();
    const auto [width, height] = backend.GetSurfaceExtent();
    const auto surfaceFormat = backend.GetSwapChainSurfaceFormat();

    TextureDesc presentTextureDesc {width, height, vk::SampleCountFlagBits::e1, surfaceFormat, ImageUsage::COLOR_ATTACHMENT, MemoryUsage::GPU_ONLY, ImageOptions::DEFAULT};

    graphBuilder.AddRenderPass("ToneMapPass", [=](PassNode* passNode){
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};
        passNode->DeclareColorAttachment("BackBuffer", presentTextureDesc, loadops, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
        
        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);
        
        RenderProcessBuilder renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =  ShaderFactory::CreateGraphicsShader("Triangle.vert.spv", "Triangle.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);
        
        passNode->graphicsShader = shader;
        passNode->pipelineState = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            cmdBuffer.Draw(3, 1);
        };
    }); 
}
}