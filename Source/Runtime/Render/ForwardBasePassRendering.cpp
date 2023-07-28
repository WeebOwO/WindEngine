#include "PassRendering.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"
#include "Runtime/Resource/Mesh.h"

namespace wind {
// Create Forward Pass and add resource
void AddForwardBasePass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();

    // Allocate shader resource
    std::shared_ptr<Buffer> cameraBuffer = std::make_shared<Buffer>(
        sizeof(CameraUnifoirmBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    std::shared_ptr<Buffer> objectBuffer = std::make_shared<Buffer>(
        sizeof(ObjectUniformBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    std::shared_ptr<Buffer> lightBuffer = std::make_shared<Buffer>(
        sizeof(LightUniformBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);

    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderBufferDesc camearaShaderBufferDesc{cameraBuffer, 0, sizeof(CameraUnifoirmBuffer)};
    ShaderBufferDesc objectShaderBufferDesc{objectBuffer, 0, sizeof(ObjectUniformBuffer)};
    ShaderBufferDesc lightBufferDesc{lightBuffer, 0, sizeof(LightUniformBuffer)};

    ShaderImageDesc albedoTextureDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};
    ShaderImageDesc normalTextureDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};
    ShaderImageDesc matalTextureDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};
    ShaderImageDesc roughTextureDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};

    ShaderImageDesc iblIrradianceTexture{nullptr, ImageUsage::SHADER_READ, BasicSampler};
    ShaderImageDesc BrdfLutTextureDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};
    ShaderImageDesc skyBoxImageDesc{nullptr, ImageUsage::SHADER_READ, BasicSampler};

    graphBuilder.AddRenderPass("OpaquePass", [=](PassNode* passNode) {
        // Setup part
        TextureOps loadops{vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment(
            "SceneColor", SceneTexture::SceneTextureDescs["SceneColor"], loadops,
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
        passNode->DeclareDepthAttachment("SceneDepth",
                                         SceneTexture::SceneTextureDescs["SceneDepth"], loadops,
                                         vk::ImageLayout::eDepthStencilAttachmentOptimal,
                                         vk::ImageLayout::eDepthStencilAttachmentOptimal);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(width, height);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> BasePassShader = ShaderFactory::CreateGraphicsShader(
            "ForwardBasePass.vert.spv", "ForwardBasePass.frag.spv");

        BasePassShader->SetShaderResource("CameraBuffer", camearaShaderBufferDesc)
            .SetShaderResource("LightBuffer", lightBufferDesc)
            .SetShaderResource("ObjectBuffer", objectShaderBufferDesc)
            .SetShaderResource("albedoTexture", albedoTextureDesc)
            .SetShaderResource("normalTexture", normalTextureDesc)
            .SetShaderResource("metallicTexture", matalTextureDesc)
            .SetShaderResource("roughnessTexture", roughTextureDesc)
            .SetShaderResource("iblIrradianceTexture", iblIrradianceTexture)
            .SetShaderResource("iblSpecBrdfLut", BrdfLutTextureDesc)
            .SetShaderResource("iblSepcTexture", skyBoxImageDesc);

        renderProcessBuilder.SetBlendState(false)
            .SetShader(BasePassShader.get())
            .SetNeedVerTex(true)
            .SetVertexFactory<Vertex>()
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = BasePassShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        // Execute part
        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto*      scene     = passNode->renderScene->GetOwnScene();
            SceneView* sceneView = passNode->renderScene;
            auto       skyBox    = scene->GetSkybox();

            auto shader = passNode->graphicsShader;

            auto& pso    = passNode->pipelineState->GetPipeline();
            auto& camera = scene->GetActiveCamera();

            glm::mat4 model = glm::mat4(1.0);

            cameraBuffer->CopyData((uint8_t*)sceneView->cameraBuffer.get(),
                                   camearaShaderBufferDesc.range, camearaShaderBufferDesc.offset);
            objectBuffer->CopyData((uint8_t*)&model, objectShaderBufferDesc.range,
                                   objectShaderBufferDesc.offset);
            lightBuffer->CopyData((uint8_t*)sceneView->lightBuffer.get(), lightBufferDesc.range,
                                  lightBufferDesc.offset);
            
            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());
                                        
            shader->Bind("iblSepcTexture", skyBox->skyBoxImage);
            shader->Bind("iblSpecBrdfLut", sceneView->iblBrdfLut);

            for (auto& gameObject : scene->GetWorld().GetWorldGameObjects()) {
                auto& model    = gameObject.model;
                auto& material = model->GetMaterial();
                
                shader->Bind("albedoTexture", material.albedoTexture);
                shader->Bind("normalTexture", material.normalTexture);
                shader->Bind("metallicTexture", material.metallicTexture);
                shader->Bind("roughnessTexture", material.roughnessTexture);
                shader->Bind("iblIrradianceTexture", sceneView->skyBoxIrradianceTexture);

                shader->FinishShaderBinding();
                model->Bind(cmdBuffer);
                model->Draw(cmdBuffer);
            }
        };
    });
}

} // namespace wind
