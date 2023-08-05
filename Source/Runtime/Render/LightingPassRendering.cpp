#include "PassRendering.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Scene/Light.h"

namespace wind {
void AddLightPass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
    // Allocate uniform buffer resource
    std::shared_ptr<Buffer> cameraBuffer = std::make_shared<Buffer>(
        sizeof(CameraUnifoirmBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    std::shared_ptr<Buffer> sunBuffer = std::make_shared<Buffer>(
        sizeof(SunUniformBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    std::shared_ptr<Buffer> lightProjectionBuffer = std::make_shared<Buffer>(
        sizeof(LightProjectionBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);

    ShaderBufferDesc camearaShaderBufferDesc{cameraBuffer, 0, sizeof(CameraUnifoirmBuffer)};
    ShaderBufferDesc sunBufferDesc{sunBuffer, 0, sizeof(SunUniformBuffer)};
    ShaderBufferDesc lightProjectionBufferDesc{lightProjectionBuffer, 0,
                                               sizeof(LightProjectionBuffer)};
    // Buffer Desc
    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    graphBuilder.AddRenderPass("LightingPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};
        TextureOps depthLoadops{vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore,
                                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};
        passNode->DeclareColorAttachment(
            "SceneColor", SceneTexture::SceneTextureDescs["SceneColor"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->DeclareDepthAttachment(
            "SceneDepth", SceneTexture::SceneTextureDescs["SceneDepth"], depthLoadops,
            vk::ImageLayout::eDepthStencilAttachmentOptimal,
            vk::ImageLayout::eDepthStencilAttachmentOptimal);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> lightShader =
            ShaderFactory::CreateGraphicsShader("FullScreen.vert.spv", "LightingPass.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(lightShader.get())
            .SetNeedVerTex(false)
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, false, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = lightShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto*      scene     = passNode->renderScene->GetOwnScene();
            SceneView* sceneView = passNode->renderScene;
            auto       skyBox    = scene->GetSkybox();

            auto& pso = passNode->pipelineState->GetPipeline();

            auto gbufferA  = graphRegister->GetResource("GBufferA");
            auto gbufferB  = graphRegister->GetResource("GBufferB");
            auto gbufferC  = graphRegister->GetResource("GBufferC");
            auto gbufferD  = graphRegister->GetResource("GBufferD");
            auto shadowMap = graphRegister->GetResource("SunShadow");

            lightShader->Bind("Sun", sunBufferDesc);
            lightShader->Bind("CameraBuffer", camearaShaderBufferDesc);

            lightShader->Bind("gbufferA",
                              {gbufferA->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferB",
                              {gbufferB->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferC",
                              {gbufferC->imageHandle, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("gbufferD",
                              {gbufferD->imageHandle, ImageUsage::SHADER_READ, BasicSampler});

            lightShader->Bind(
                "iblSepcTexture",
                ShaderImageDesc{skyBox->skyBoxImage, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind(
                "iblSpecBrdfLut",
                ShaderImageDesc{sceneView->iblBrdfLut, ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("iblIrradianceTexture", {sceneView->skyBoxIrradianceTexture,
                                                       ImageUsage::SHADER_READ, BasicSampler});
            lightShader->Bind("LightProjection", lightProjectionBufferDesc);

            lightShader->Bind("shadowMap",
                              {shadowMap->imageHandle, ImageUsage::SHADER_READ, BasicSampler});

            lightShader->Bind("PointLights", {sceneView->pointLightBuffers, 0,
                                              sceneView->pointLightBuffers->GetByteSize()});
            // update uniform data
            cameraBuffer->CopyData((uint8_t*)sceneView->cameraBuffer.get(),
                                   camearaShaderBufferDesc.range, camearaShaderBufferDesc.offset);

            sunBuffer->CopyData((uint8_t*)sceneView->sunBuffer.get(), sunBufferDesc.range,
                                sunBufferDesc.offset);

            lightProjectionBuffer->CopyData((uint8_t*)sceneView->lightProjectionBuffer.get(),
                                            lightProjectionBufferDesc.range,
                                            lightProjectionBufferDesc.offset);
            auto& pointLightArray = scene->GetPointLightArray();
            sceneView->pointLightBuffers->CopyData((uint8_t*)pointLightArray.data(),
                                                   sizeof(PointLight) * pointLightArray.size(), 0);
            int pointLightCnt = scene->GetPointLightCnt();
            cmdBuffer.PushConstant(passNode, &pointLightCnt);

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        lightShader->GetDescriptorSet());

            cmdBuffer.Draw(3, 1);
        };
    });
}
} // namespace wind