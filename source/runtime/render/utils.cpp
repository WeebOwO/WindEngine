#include "runtime/render/utils.h"

#include <cstdint>

#include "runtime/render/context.h"

namespace wind::utils {
vk::ShaderModule CreateShaderModule(const std::vector<char>& shaderCode) {
    vk::ShaderModuleCreateInfo createInfo;
    vk::ShaderModule           shaderModule;
    createInfo.setCodeSize(shaderCode.size())
        .setPCode(reinterpret_cast<const uint32_t*>(shaderCode.data()));
    shaderModule = RenderContext::GetInstace().device.createShaderModule(createInfo);
    return shaderModule;
}

vk::Pipeline CreateGraphicsPipelines(const vk::GraphicsPipelineCreateInfo& createinfo) {
    auto result = RenderContext::GetInstace().device.createGraphicsPipeline(nullptr, createinfo);
    if (result.result != vk::Result::eSuccess) {
        std::cout << "create graphics pipeline failed: " << result.result << std::endl;
    }
    return result.value;
}

vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createinfo) {
    return RenderContext::GetInstace().device.createPipelineLayout(createinfo);
}

vk::RenderPass CreateRenderPass(vk::RenderPassCreateInfo createInfo) {
    return RenderContext::GetInstace().device.createRenderPass(createInfo);
}


} // namespace wind::utils