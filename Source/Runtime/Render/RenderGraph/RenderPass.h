#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

#include "Runtime/Render/RHI/Shader.h"

namespace wind {

class RenderProcessBuilder {
public:
    friend class RenderProcess;
    RenderProcess Build();
private:
    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<vk::DescriptorSet> m_descriptorSets;
};

class RenderProcess {
public:
    struct Pipeline {
        vk::Pipeline          pipeline;
        vk::PipelineLayout    pipelineLayout;
        vk::PipelineBindPoint bindPoint;
    };
    void Init(vk::ShaderModule vertexShaderModule, vk::ShaderModule fragShaderModule);
    void Init(const GraphicsShader& graphicsShader);
private:
    Pipeline m_pipeline;
    std::vector<vk::DescriptorSet> m_descriptorSets;
};
}; // namespace wind