#include "Shader.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <unordered_map>

#include "Runtime/Base/Io.h"
#include "Runtime/Render/RHI/Backend.h"

namespace wind {
std::unordered_map<std::string, std::shared_ptr<GraphicsShader>> ShaderFactory::m_shaderCache{};

void GraphicsShader::GenerateVulkanDescriptorSetLayout() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    
    for (const auto& [set, resourceNameVecs] : m_setGroups) {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo; 
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

        for(const auto& resoueceName : resourceNameVecs) {
            BindMetaData metaData = m_reflectionDatas[resoueceName];
            vk::DescriptorSetLayoutBinding binding;
            vk::ShaderStageFlags stageFlags = metaData.shaderStageFlags.front();
            binding.setBinding(metaData.binding)
                   .setDescriptorCount(1)
                   .setDescriptorType(metaData.descriptorType)
                   .setStageFlags(stageFlags);
            layoutBindings.push_back(binding);
        }

        descriptorSetLayoutCreateInfo.setBindingCount(layoutBindings.size())
                                     .setBindings(layoutBindings);
        m_DescriptorSetLayouts.push_back(device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
    }
}

void GraphicsShader::CollectSpirvMetaData(std::vector<uint32_t> spivrBinary,
                                          vk::ShaderStageFlags  shaderFlags) {
    const auto& device = RenderBackend::GetInstance().GetDevice();

    spirv_cross::CompilerGLSL    compiler(std::move(spivrBinary));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    for (auto& resource : resources.uniform_buffers) {
        if (m_reflectionDatas.find(resource.name) == m_reflectionDatas.end()) {
            uint32_t     set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t     binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            BindMetaData metaData{binding, vk::DescriptorType::eUniformBuffer};
            WIND_INFO("Here is uniform buffer, set is {}, binding is {}", set, binding);
            m_reflectionDatas[resource.name] = metaData;
            m_setGroups[set].push_back(resource.name);
        }
        m_reflectionDatas[resource.name].shaderStageFlags.push_back(shaderFlags);
    }
}

GraphicsShader::~GraphicsShader() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.destroyShaderModule(m_vertexShader);
    device.destroyShaderModule(m_fragShader);

    // destroy our layout
    for(auto& layout : m_DescriptorSetLayouts) {
        device.destroyDescriptorSetLayout(layout);
    }
}

GraphicsShader::GraphicsShader(std::string_view vertexShaderfilePath,
                               std::string_view fragmentShaderFilePath) {
    auto& device = RenderBackend::GetInstance().GetDevice();

    auto spirvVertexBinary = io::ReadSpirvBinaryFile(vertexShaderfilePath);
    auto spirvFragBinary   = io::ReadSpirvBinaryFile(fragmentShaderFilePath);

    // Collect shader meta data
    CollectSpirvMetaData(spirvVertexBinary, vk::ShaderStageFlagBits::eVertex);
    CollectSpirvMetaData(spirvFragBinary, vk::ShaderStageFlagBits::eFragment);

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setPCode(spirvVertexBinary.data())
        .setCodeSize(spirvVertexBinary.size() * sizeof(uint32_t));
    m_vertexShader = device.createShaderModule(createInfo);

    createInfo.setPCode(spirvFragBinary.data())
        .setCodeSize(spirvFragBinary.size() * sizeof(uint32_t));

    m_fragShader = device.createShaderModule(createInfo);

    GenerateVulkanDescriptorSetLayout();
}

std::shared_ptr<GraphicsShader>
ShaderFactory::CreateGraphicsShader(const std::string& name, const std::string& vertexFilePath,
                                    const std::string& fragFilePath) {
    if (m_shaderCache.find(name) != m_shaderCache.end()) { return m_shaderCache[name]; }
    return m_shaderCache[name] = std::make_shared<GraphicsShader>(vertexFilePath, fragFilePath);
}

} // namespace wind