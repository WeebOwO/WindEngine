#pragma once

#include "lve_device.h"

// std
#include <string>
#include <vector>

namespace lve {

struct PipelineConfigInfo {
    PipelineConfigInfo()                                     = default;
    PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkViewport                             viewport;
    VkRect2D                               scissor;
    VkPipelineViewportStateCreateInfo      viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo   multisampleInfo;
    VkPipelineColorBlendAttachmentState    colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
    VkPipelineLayout                       pipelineLayout = nullptr;
    VkRenderPass                           renderPass     = nullptr;
    uint32_t                               subpass        = 0;
};

class LvePipeline {
public:
    LvePipeline(LveDevice& device, const std::string& vertFilepath, const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo);
    ~LvePipeline();

    LvePipeline(const LvePipeline&)    = delete;
    void operator=(const LvePipeline&) = delete;

    void Bind(VkCommandBuffer commandBuffer);

    static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width,
                                          uint32_t height);

private:
    static std::vector<char> ReadFile(const std::string& filepath);

    void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
                                const PipelineConfigInfo& configInfo);

    void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    LveDevice&     m_lveDevice;
    VkPipeline     m_graphicsPipeline;
    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;
};
} // namespace lve