#include "PassRendering.h"

#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
void AddShadowPass(RenderGraphBuilder& graphBuilder) {
    // Allocate shader resource

    std::shared_ptr<Buffer> lightProjectionBuffer = std::make_shared<Buffer>(
        sizeof(LightProjectionBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);

    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    ShaderBufferDesc lightProjectionBufferDesc{lightProjectionBuffer, 0,
                                               sizeof(LightProjectionBuffer)};

    TextureDesc dummy{SceneView::ShadowMapResolutionX,
                      SceneView::ShadowMapResolutionY,
                      vk::SampleCountFlagBits::e1,
                      vk::Format::eR8G8B8A8Snorm,
                      ImageUsage::COLOR_ATTACHMENT,
                      MemoryUsage::GPU_ONLY,
                      ImageOptions::DEFAULT};

    graphBuilder.AddRenderPass("ShadowPass", [=](PassNode* passNode) {
        TextureOps loadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                           vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        TextureOps depthloadops{vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
                                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare};

        passNode->DeclareColorAttachment("Dummy", dummy, loadops, vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eColorAttachmentOptimal);

        passNode->DeclareDepthAttachment("SunShadow", SceneView::sunShadowDesc, depthloadops,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::eShaderReadOnlyOptimal);

        passNode->CreateRenderPass();
        passNode->SetRenderRect(SceneView::ShadowMapResolutionX, SceneView::ShadowMapResolutionY);

        RenderProcessBuilder renderProcessBuilder;

        std::shared_ptr<GraphicsShader> shadowPassShader =
            ShaderFactory::CreateGraphicsShader("Shadow.vert.spv", "Shadow.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(shadowPassShader.get())
            .SetVertexFactory<gltf::GLTFVertex>()
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = shadowPassShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto*      scene     = passNode->renderScene->GetOwnScene();
            SceneView* sceneView = passNode->renderScene;

            auto& sponzaMesh = scene->GetRequiredGLTFModel("Sponza");
            auto& pso        = passNode->pipelineState->GetPipeline();

            shadowPassShader->Bind("LightProjection", lightProjectionBufferDesc);

            lightProjectionBuffer->CopyData((uint8_t*)sceneView->lightProjectionBuffer.get(),
                                            lightProjectionBufferDesc.range,
                                            lightProjectionBufferDesc.offset);

            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        shadowPassShader->GetDescriptorSet());

            for (auto& subMesh : sponzaMesh.submeshes) {
                size_t indexCount = subMesh.indexBuffer.GetByteSize() / sizeof(uint32_t);

                cmdBuffer.BindVertexBuffers(subMesh.vertexBuffer);
                cmdBuffer.BindIndexBufferUInt32(subMesh.indexBuffer);
                cmdBuffer.DrawIndexed(indexCount, 1);
            }
        };
    });
}
} // namespace wind