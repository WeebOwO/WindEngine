#include "PassRendering.h"

#include "Runtime/Render/RHI/Sampler.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Resource/GLTFLoader.h"
#include "Runtime/Resource/Mesh.h"
#include <stdint.h>

namespace wind {
void AddDeferedBasePass(RenderGraphBuilder& graphBuilder) {
    const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
    // Allocate shader resource
    std::shared_ptr<Buffer> cameraBuffer = std::make_shared<Buffer>(
        sizeof(CameraUnifoirmBuffer), BufferUsage::UNIFORM_BUFFER, MemoryUsage::CPU_TO_GPU);
    // Buffer Desc
    ShaderBufferDesc         camearaShaderBufferDesc{cameraBuffer, 0, sizeof(CameraUnifoirmBuffer)};
    std::shared_ptr<Sampler> BasicSampler =
        std::make_shared<Sampler>(Sampler::MinFilter::LINEAR, Sampler::MagFilter::LINEAR,
                                  Sampler::AddressMode::REPEAT, Sampler::MipFilter::LINEAR);

    graphBuilder.AddRenderPass("BasePass", [=](PassNode* passNode) {
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

        std::shared_ptr<GraphicsShader> BasePassShader =
            ShaderFactory::CreateGraphicsShader("BasePass.vert.spv", "BasePass.frag.spv");

        renderProcessBuilder.SetBlendState(false)
            .SetShader(BasePassShader.get())
            .SetVertexFactory<gltf::GLTFVertex>()
            .SetRenderPass(passNode->renderPass)
            .SetDepthSetencilTestState(true, true, false, vk::CompareOp::eLessOrEqual);

        passNode->graphicsShader = BasePassShader;
        passNode->pipelineState  = renderProcessBuilder.BuildGraphicProcess();

        return [=](CommandBuffer& cmdBuffer, RenderGraphRegister* graphRegister) {
            auto*      scene      = passNode->renderScene->GetOwnScene();
            SceneView* sceneView  = passNode->renderScene;
            auto&      sponzaMesh = scene->GetRequiredGLTFModel("Sponza");

            BasePassShader->Bind("CameraBuffer", camearaShaderBufferDesc);
            BasePassShader->Bind("MaterialBuffer", {sponzaMesh.materialBuffer, 0,
                                                    sizeof(gltf::GLTFMesh::Material) *
                                                        gltf::GLTFMesh::MaxMaterialCount});
            BasePassShader->Bind("textureSampler", BasicSampler);
            BasePassShader->Bind("textureArray", sponzaMesh.textures);

            struct ConstantData {
                uint32_t materialIndex;
            };

            auto& pso = passNode->pipelineState->GetPipeline();

            cameraBuffer->CopyData((uint8_t*)sceneView->cameraBuffer.get(),
                                   camearaShaderBufferDesc.range, camearaShaderBufferDesc.offset);

            // cmdBuffer.PushConstant(passNode, &constantData);
            cmdBuffer.BindDescriptorSet(pso.bindPoint, pso.pipelineLayout,
                                        BasePassShader->GetDescriptorSet());

            for (auto& subMesh : sponzaMesh.submeshes) {
                size_t indexCount = subMesh.indexBuffer.GetByteSize() / sizeof(uint32_t);

                ConstantData constantData{subMesh.materialIndex};
                cmdBuffer.PushConstant(passNode, &constantData);
                cmdBuffer.BindVertexBuffers(subMesh.vertexBuffer);
                cmdBuffer.BindIndexBufferUInt32(subMesh.indexBuffer);
                cmdBuffer.DrawIndexed(indexCount, 1);
            }
        };
    });
}
} // namespace wind