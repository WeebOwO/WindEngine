#include "PassRendering.h"

namespace wind {

void AddSkyboxPass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

    std::shared_ptr<Buffer> skyBoxBuffer = std::make_shared<Buffer>(
        sizeof(SkyBoxUniformBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);

    ShaderBufferDesc skyboxBufferDesc{skyBoxBuffer, 0, sizeof(SkyBoxUniformBuffer)};

    static std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderImageDesc skyBoxImageDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};

    graphBuilder.AddRenderPass("SkyBoxPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment(
            "SceneColor", SceneTexture::SceneTextureDescs["SceneColor"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
        passNode->DeclareDepthAttachment(
            "SceneDepth", SceneTexture::SceneTextureDescs["SceneDepth"], loadops,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
            
        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> skyPassShader =
            ShaderFactory::CreateGraphicsShader("SkyBox.vert.spv", "SkyBox.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetNeedVerTex(false)
            .SetShader(skyPassShader.get())
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, false, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = skyPassShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto* scene = passNode->renderScene->GetOwnScene();

            auto       skyBox    = scene->GetSkybox();
            SceneView* sceneView = passNode->renderScene;

            auto shader = passNode->graphicsShader;

            auto& pso    = passNode->pipelineState->GetPipeline();
            auto& camera = scene->GetActiveCamera();

            // Finish Binding shader
            shader->Bind("SkyBoxBuffer", skyboxBufferDesc);
            shader->Bind("SkyboxCubemap", {skyBox->skyBoxImage, ImageUsage::SHADER_READ, BasicSampler});

            glm::mat4 model = glm::mat4(1.0);

            skyBoxBuffer->CopyData((uint8_t*)sceneView->skyBoxBuffer.get(), skyboxBufferDesc.range,
                                   skyboxBufferDesc.offset);

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            // draw the skyboxs
            cmdBuffer.Draw(36, 1);
            // Not end here
        };
    });
}
} // namespace wind