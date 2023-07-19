#include "PassRendering.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Sampler.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

#include <memory>

namespace wind {
void AddToneMappingCombinePass(RenderGraphBuilder& graphBuilder) {
    auto& backend = RenderBackend::GetInstance();

    const auto [width, height] = backend.GetSurfaceExtent();
    const auto surfaceFormat   = backend.GetSwapChainSurfaceFormat();

    TextureDesc presentTextureDesc{width,
                                   height,
                                   vk::SampleCountFlagBits::e1,
                                   surfaceFormat,
                                   ImageUsage::COLOR_ATTACHMENT,
                                   MemoryUsage::GPU_ONLY,
                                   ImageOptions::DEFAULT};

    std::shared_ptr<Sampler> sampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderImageDesc sceneColorDesc{nullptr, ImageUsage::SHADER_READ, sampler};
    ShaderImageDesc bloomColorDesc{nullptr, ImageUsage::SHADER_READ, sampler};

    graphBuilder.AddRenderPass("ToneMapPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};
        passNode->DeclareColorAttachment("BackBuffer", presentTextureDesc, loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::ePresentSrcKHR);

        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder            renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "ToneMapping.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);

        shader->SetShaderResource("sceneColor", sceneColorDesc)
            .SetShaderResource("bloomCombine", bloomColorDesc);

        passNode->graphicsShader = shader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto sceneColor = graphRegister->GetResource("SceneColor");
            auto bloomColor = graphRegister->GetResource("BloomBlurY");
            shader->Bind("sceneColor", sceneColor->imageHandle);
            shader->Bind("bloomCombine", bloomColor->imageHandle);
            shader->FinishShaderBinding();
            auto& pso = passNode->pipelineState->GetPipeline();

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            cmdBuffer.Draw(3, 1);
        };
    });
}

void AddDeferToneMappingCombinePass(RenderGraphBuilder& graphBuilder) {
    auto& backend = RenderBackend::GetInstance();

    const auto [width, height] = backend.GetSurfaceExtent();
    const auto surfaceFormat   = backend.GetSwapChainSurfaceFormat();

    TextureDesc presentTextureDesc{width,
                                   height,
                                   vk::SampleCountFlagBits::e1,
                                   surfaceFormat,
                                   ImageUsage::COLOR_ATTACHMENT,
                                   MemoryUsage::GPU_ONLY,
                                   ImageOptions::DEFAULT};

    std::shared_ptr<Sampler> sampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderImageDesc sceneColorDesc{nullptr, ImageUsage::SHADER_READ, sampler};

    graphBuilder.AddRenderPass("ToneMapPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};
                           
        passNode->DeclareColorAttachment("BackBuffer", presentTextureDesc, loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::ePresentSrcKHR);

        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder            renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "ToneMappingDefer.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);

        shader->SetShaderResource("sceneColor", sceneColorDesc);

        passNode->graphicsShader = shader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto sceneColor = graphRegister->GetResource("SceneColor");
            shader->Bind("sceneColor", sceneColor->imageHandle);
            shader->FinishShaderBinding();
            auto& pso = passNode->pipelineState->GetPipeline();

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            cmdBuffer.Draw(3, 1);
        };
    });
}
} // namespace wind