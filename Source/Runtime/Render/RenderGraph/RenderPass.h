#pragma once

#include <functional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Shader.h"

namespace wind {
class RenderProcess;

struct TextureOps {
    vk::AttachmentLoadOp  load;
    vk::AttachmentStoreOp store;
    vk::AttachmentLoadOp  stencilLoad;
    vk::AttachmentStoreOp stencilStore;
};

// old version
class RenderProcessBuilder {
public:
    RenderProcessBuilder& SetShader(GraphicsShader* graphicsShader);
    RenderProcessBuilder& SetBlendState(bool blendEnable);
    RenderProcessBuilder& SetDepthSetencilTestState(bool depthTestEnable, bool depthWriteEnable,
                                                    bool          stencilTestEnable,
                                                    vk::CompareOp depthCompareMode);
    RenderProcessBuilder& SetRenderPass(vk::RenderPass renderPass);
    RenderProcessBuilder& SetNeedVerTex(bool condition);
    template <typename VertexFactory> 
    RenderProcessBuilder& SetVertexFactory() {
        m_vertexAttributeDescriptions = VertexFactory::GetVertexInputAttributeDescriptions();
        m_vertexInputBinding          = VertexFactory::GetInputBindingDescription();
        return *this;
    }
    std::shared_ptr<RenderProcess> BuildGraphicProcess();

private:
    vk::RenderPass m_renderPass;
    // shader stage createInfo;
    std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
    // vertexInput state
    std::vector<vk::VertexInputAttributeDescription> m_vertexAttributeDescriptions;
    vk::VertexInputBindingDescription                m_vertexInputBinding;
    // depth and stencil state info
    vk::PipelineDepthStencilStateCreateInfo m_depthStencilStateCreateInfo;
    // extra blend state setting
    vk::PipelineColorBlendAttachmentState m_colorBlendAttachment;
    vk::PipelineColorBlendStateCreateInfo m_PipelineColorBlendStateCreateInfo;
    // pipelineLayoutCreateInfo
    vk::PipelineLayoutCreateInfo m_pipelineLayoutCreateInfo{};

    bool m_needVertexData = true;
};

class RenderProcess {
public:
    friend class RenderProcessBuilder;
    RenderProcess(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout,
                  vk::PipelineBindPoint bindPoint)
        : m_pipeline({pipeline, pipelineLayout, bindPoint}) {}
    ~RenderProcess();

    struct Pipeline {
        vk::Pipeline          pipeline;
        vk::PipelineLayout    pipelineLayout;
        vk::PipelineBindPoint bindPoint;
    };

    [[nodiscard]] auto& GetPipeline() { return m_pipeline; }

private:
    Pipeline m_pipeline;
};
}; // namespace wind