#include "runtime/render/rhi/shader.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <unordered_map>

#include "Runtime/Base/Io.h"
#include "Runtime/Render/Rhi/Backend.h"

namespace wind {

void GraphicsShader::CollectSpirvMetaData(std::vector<uint32_t> spivrBinary) {
    const auto& device = RenderBackend::GetInstance().GetDevice();
    spirv_cross::CompilerGLSL    compiler(std::move(spivrBinary));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    std::unordered_map<int, std::vector<vk::DescriptorSetLayoutBinding>> bindingMaps;

    for (auto& resource : resources.uniform_buffers) {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding;
        uint32_t set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

        descriptorSetLayoutBinding.setBinding(binding)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex & vk::ShaderStageFlagBits::eFragment);
        bindingMaps[set].push_back(descriptorSetLayoutBinding);

        WIND_INFO("Uniform set is {}, binding is {}", set, binding);
    }

    for(const auto& [key, value] : bindingMaps) {
        vk::DescriptorSetLayoutCreateInfo createInfo;
        createInfo.setBindingCount(value.size())
                  .setBindings(value);
        m_descriptorSetLayoutMaps[key] = device.createDescriptorSetLayout(createInfo);
    }
}

GraphicsShader::~GraphicsShader() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.destroyShaderModule(m_vertexShader);
    device.destroyShaderModule(m_fragShader);
    
    for(auto& [key, value] : m_descriptorSetLayoutMaps) {
        device.destroyDescriptorSetLayout(value);
    }
}

GraphicsShader::GraphicsShader(
    std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath,
    vk::VertexInputBindingDescription                       inputBinding,
    const std::vector<vk::VertexInputAttributeDescription>& inputAttributeDescriptions) {

    auto& device = RenderBackend::GetInstance().GetDevice();
    auto  size   = inputAttributeDescriptions.size();
    m_inputAttributeDescription.reserve(size);
    for (auto inputAttribute : inputAttributeDescriptions) {
        m_inputAttributeDescription.push_back(inputAttribute);
    }
    m_inputBindingDescription = inputBinding;

    auto spirvVertexBinary = io::ReadSpirvBinaryFile(vertexShaderfilePath);
    auto spirvFragBinary   = io::ReadSpirvBinaryFile(fragmentShaderFilePath);

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setPCode(spirvVertexBinary.data())
        .setCodeSize(spirvVertexBinary.size() * sizeof(uint32_t));
    m_vertexShader = device.createShaderModule(createInfo);

    createInfo.setPCode(spirvFragBinary.data())
        .setCodeSize(spirvFragBinary.size() * sizeof(uint32_t));

    m_fragShader = device.createShaderModule(createInfo);
}

} // namespace wind