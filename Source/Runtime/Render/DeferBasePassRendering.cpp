#include "PassRendering.h"

#include "Runtime/Render/RHI/Shader.h"

namespace wind {
void AddDeferedBasePass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
    graphBuilder.AddRenderPass("BasePass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment(
            "SceneColor", SceneTexture::SceneTextureDescs["SceneColor"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
        
        passNode->DeclareDepthAttachment("SceneDepth",
                                         SceneTexture::SceneTextureDescs["SceneDepth"], loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eDepthStencilAttachmentOptimal);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> BasePassShader =
            ShaderFactory::CreateGraphicsShader("BasePass.vert.spv", "BasePass.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(BasePassShader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = BasePassShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            cmdBuffer.Draw(3, 1);
            
        };
    });
}
} // namespace wind