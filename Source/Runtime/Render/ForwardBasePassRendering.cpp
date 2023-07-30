#include "PassRendering.h"
#include "Runtime/Render/RHI/Shader.h"
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
        sizeof(SunUniformBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);

    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderBufferDesc camearaShaderBufferDesc{cameraBuffer, 0, sizeof(CameraUnifoirmBuffer)};
    ShaderBufferDesc objectShaderBufferDesc{objectBuffer, 0, sizeof(ObjectUniformBuffer)};
    ShaderBufferDesc lightBufferDesc{lightBuffer, 0, sizeof(SunUniformBuffer)};

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
            lightBuffer->CopyData((uint8_t*)sceneView->sunBuffer.get(), lightBufferDesc.range,
                                  lightBufferDesc.offset);

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shader->GetDescriptorSet());

            shader->Bind("iblSepcTexture", ShaderImageDesc{skyBox->skyBoxImage,
                                                           ImageUsage::SHADER_READ, BasicSampler});
            shader->Bind("iblSpecBrdfLut", ShaderImageDesc{sceneView->iblBrdfLut,
                                                           ImageUsage::SHADER_READ, BasicSampler});
            shader->Bind("iblIrradianceTexture", {sceneView->skyBoxIrradianceTexture,
                                                  ImageUsage::SHADER_READ, BasicSampler});

            for (auto& gameObject : scene->GetWorld().GetWorldGameObjects()) {
                auto& model    = gameObject.model;
                auto& material = model->GetMaterial();

                // Get shader binding
                shader->Bind("CameraBuffer", camearaShaderBufferDesc);
                shader->Bind("LightBuffer", lightBufferDesc);
                shader->Bind("ObjectBuffer", objectShaderBufferDesc);
                shader->Bind("albedoTexture",
                             {material.albedoTexture, ImageUsage::SHADER_READ, BasicSampler});
                shader->Bind("normalTexture",
                             {material.normalTexture, ImageUsage::SHADER_READ, BasicSampler});
                shader->Bind("metallicTexture",
                             {material.metallicTexture, ImageUsage::SHADER_READ, BasicSampler});
                shader->Bind("roughnessTexture",
                             {material.roughnessTexture, ImageUsage::SHADER_READ, BasicSampler});

                model->Bind(cmdBuffer);
                model->Draw(cmdBuffer);
            }
        };
    });
}

} // namespace wind
