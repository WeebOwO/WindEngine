#pragma once

#include "Runtime/Render/RHI/CommandBuffer.h"
#include "vulkan/vulkan.hpp"

#include <functional>
#include <vector>

#include "Runtime/Render/RHI/Shader.h"

namespace wind {
class RenderProcess;

class RenderProcessBuilder {
public:
    RenderProcessBuilder& SetShader(GraphicsShader graphicsShader);
    RenderProcessBuilder& SetShader(const std::string& vertexShaderFile, const std::string& fragShaderFile);
    RenderProcessBuilder& SetBlendState(bool blendEnable);
    RenderProcessBuilder& SetDepthSetencilTestState(bool depthTestEnable, bool depthWriteEnable,
                                                    bool          stencilTestEnable,
                                                    vk::CompareOp depthCompareMode);
    RenderProcessBuilder& SetRenderPass(vk::RenderPass renderPass);
    std::shared_ptr<RenderProcess> BuildGraphicProcess();
    
private:
    vk::RenderPass m_renderPass;
    // shader stage createInfo;
    std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
    // depth and stencil state info
    vk::PipelineDepthStencilStateCreateInfo m_depthStencilStateCreateInfo;
    // extra blend state setting
    vk::PipelineColorBlendStateCreateInfo m_PipelineColorBlendStateCreateInfo;
    // pipelineLayoutCreateInfo
    vk::PipelineLayoutCreateInfo m_pipelineLayoutCreateInfo;
};

class RenderProcess {
public:
    friend class RenderProcessBuilder;
    RenderProcess(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout, vk::PipelineBindPoint bindPoint)
    :m_pipeline({pipeline, pipelineLayout, bindPoint}) {}

    struct Pipeline {
        vk::Pipeline          pipeline;
        vk::PipelineLayout    pipelineLayout;
        vk::PipelineBindPoint bindPoint;
    };
private:
    Pipeline m_pipeline;
};
}; // namespace wind