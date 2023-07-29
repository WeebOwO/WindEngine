#include "PassRendering.h"
#include "Runtime/Render/RHI/Image.h"

namespace wind {
void AddLightPass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
    // Allocate shader resource
    std::shared_ptr<Buffer> cameraBuffer = std::make_shared<Buffer>(
        sizeof(CameraUnifoirmBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    // Buffer Desc
    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);
    
    graphBuilder.AddRenderPass("LightingPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment(
            "SceneColor", SceneTexture::SceneTextureDescs["SceneColor"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->DeclareDepthAttachment(
            "SceneDepth", SceneTexture::SceneTextureDescs["SceneDepth"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> lightShader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "LightingPass.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(lightShader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(false, false, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = lightShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto*      scene      = passNode->renderScene->GetOwnScene();
            auto& pso    = passNode->pipelineState->GetPipeline();

            auto gbufferA = graphRegister->GetResource("GBufferA");
            auto gbufferB = graphRegister->GetResource("GBufferB");
            auto gbufferC = graphRegister->GetResource("GBufferC");
            auto gbufferD = graphRegister->GetResource("GBufferD");

            lightShader->Bind("gbufferA", {gbufferA->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferB", {gbufferB->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferC", {gbufferC->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferD", {gbufferD->imageHandle, ImageUsage::SHADER_READ, BasicSampler});

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        lightShader->GetDescriptorSet());

            SceneView* sceneView  = passNode->renderScene;

            cmdBuffer.Draw(3, 1);
        };
    });
}
} // namespace wind