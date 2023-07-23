#include "PassRendering.h"

#include <memory>
#include <stdint.h>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

namespace wind {
void AddBloomSetupPass(RenderGraphBuilder& graphBuilder) {
    auto& backend              = RenderBackend::GetInstance();
    const auto [width, height] = backend.GetSurfaceExtent();

    TextureDesc bloomSetupTextureDesc{width,
                                      height,
                                      vk::SampleCountFlagBits::e1,
                                      vk::Format::eR16G16B16A16Sfloat,
                                      ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                                      MemoryUsage::GPU_ONLY,
                                      ImageOptions::DEFAULT};

    std::shared_ptr<Sampler> sampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderImageDesc sceneColorDesc{nullptr, ImageUsage::SHADER_READ, sampler};
    
    graphBuilder.AddRenderPass("BloomSetupPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment("BloomSetup", bloomSetupTextureDesc, loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder            renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "BloomSetup.frag.spv");

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

void AddBloomBlurPass(RenderGraphBuilder& graphBuilder) {
    auto& backend              = RenderBackend::GetInstance();
    const auto [width, height] = backend.GetSurfaceExtent();

    TextureDesc bloomBlurTextureDesc{width,
                                     height,
                                     vk::SampleCountFlagBits::e1,
                                     vk::Format::eR16G16B16A16Sfloat,
                                     ImageUsage::COLOR_ATTACHMENT | ImageUsage::SHADER_READ,
                                     MemoryUsage::GPU_ONLY,
                                     ImageOptions::DEFAULT};

    std::shared_ptr<Sampler> sampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderImageDesc bloomSetupDesc{nullptr, ImageUsage::SHADER_READ, sampler};

    graphBuilder.AddRenderPass("BloomBlurX", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment("BloomBlurX", bloomBlurTextureDesc, loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder            renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "BloomBlurX.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);

        shader->SetShaderResource("bloomSetup", bloomSetupDesc);

        passNode->graphicsShader = shader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto bloomSetup = graphRegister->GetResource("BloomSetup");
            shader->Bind("bloomSetup", bloomSetup->imageHandle);

            shader->FinishShaderBinding();

            auto& pso = passNode->pipelineState->GetPipeline();

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            cmdBuffer.Draw(3, 1);
        };
    });

    graphBuilder.AddRenderPass("BloomBlurY", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment("BloomBlurY", bloomBlurTextureDesc, loadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->isWriteToDepth = false;
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder            renderProcessBuilder;
        std::shared_ptr<GraphicsShader> shader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "BloomBlurY.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);

        shader->SetShaderResource("bloomSetup", bloomSetupDesc);
          
        passNode->graphicsShader = shader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto bloomSetup = graphRegister->GetResource("BloomBlurX");
            shader->Bind("bloomSetup", bloomSetup->imageHandle);
            shader->FinishShaderBinding();

            auto& pso = passNode->pipelineState->GetPipeline();

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            cmdBuffer.Draw(3, 1);
        };
    });
}
} // namespace wind