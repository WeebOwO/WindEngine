#include "Shader.h"

#include <map>
#include <memory>
#include <optional>
#include <spirv_cross/spirv_glsl.hpp>
#include <unordered_map>

#include "Runtime/Base/Io.h"
#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {

void GraphicsShader::GenerateVulkanDescriptorSetLayout() {
    auto& device    = RenderBackend::GetInstance().GetDevice();
    auto& allocater = RenderBackend::GetInstance().GetDescriptorAllocator();

    std::vector<vk::DescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos;
    std::vector<vk::DescriptorSetLayoutBinding>    layoutBindings;

    std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> m_setGroups;

    for (const auto& [resourceName, metaData] : m_reflectionDatas) {
        vk::DescriptorSetLayoutBinding binding;
        vk::ShaderStageFlags           stageFlags = metaData.shaderStageFlag;
        uint32_t                       set        = metaData.set;

        binding.setBinding(metaData.binding)
            .setDescriptorCount(metaData.count)
            .setDescriptorType(metaData.descriptorType)
            .setStageFlags(stageFlags);

        layoutBindings.push_back(binding);
        m_setGroups[set].push_back(binding);
    }

    for (const auto& [setIndex, bindingVecs] : m_setGroups) {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
        descriptorSetLayoutCreateInfo.setBindingCount(bindingVecs.size()).setBindings(bindingVecs);
        vk::DescriptorSetLayout setLayout =
            device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

        m_descriptorSetLayouts.push_back(setLayout);
        m_descriptorSets.push_back(allocater->Allocate(setLayout));
    }
}

void GraphicsShader::GeneratePushConstantData() {
    if (m_pushConstantMeta.has_value()) {
        vk::PushConstantRange range;
        range.setOffset(m_pushConstantMeta->offset)
            .setSize(m_pushConstantMeta->size)
            .setStageFlags(m_pushConstantMeta->shadeshaderStageFlag);
        m_pushConstantRange = std::optional<vk::PushConstantRange>(range);
    }
}

void GraphicsShader::CollectSpirvMetaData(std::vector<uint32_t> spivrBinary,
                                          vk::ShaderStageFlags  shaderFlags) {
    const auto& device = RenderBackend::GetInstance().GetDevice();

    spirv_cross::CompilerGLSL    compiler(std::move(spivrBinary));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    auto collectResource = [&](auto resource, vk::DescriptorType descriptorType) {
        if (m_reflectionDatas.find(resource.name) == m_reflectionDatas.end()) {
            std::string_view resourceName = resource.name;
            uint32_t set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            const spirv_cross::SPIRType& type          = compiler.get_type(resource.type_id);
            uint32_t                     typeArraySize = type.array.size();
            uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
            BindMetaData                 metaData{set, binding, count, descriptorType, shaderFlags};
            m_reflectionDatas[resource.name] = metaData;
        } else {
            m_reflectionDatas[resource.name].shaderStageFlag |= shaderFlags;
        }
    };

    for (auto& resource : resources.uniform_buffers) {
        collectResource(resource, vk::DescriptorType::eUniformBuffer);
    }

    for (auto& resource : resources.sampled_images) {
        collectResource(resource, vk::DescriptorType::eCombinedImageSampler);
    }

    for (auto& resource : resources.separate_samplers) {
        collectResource(resource, vk::DescriptorType::eSampler);
    }

    for (auto& resource : resources.separate_images) {
        collectResource(resource, vk::DescriptorType::eSampledImage);
    }
    
    for (const auto& resource : resources.push_constant_buffers) {
        std::string_view             resourceName = resource.name;
        const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
        uint32_t                     size         = compiler.get_declared_struct_size(type);
        if (!m_pushConstantMeta.has_value()) {
            PushConstantMetaData meta{size, 0, shaderFlags};
            m_pushConstantMeta = std::optional<PushConstantMetaData>(meta);
        } else {
            m_pushConstantMeta->shadeshaderStageFlag |= shaderFlags;
        }
    }
}

GraphicsShader::~GraphicsShader() {
    auto& device = RenderBackend::GetInstance().GetDevice();

    device.destroyShaderModule(m_vertexShader);
    device.destroyShaderModule(m_fragShader);

    // destroy our layout
    for (auto& descriptorSetLayout : m_descriptorSetLayouts) {
        device.destroyDescriptorSetLayout(descriptorSetLayout);
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
    GeneratePushConstantData();
}

void GraphicsShader::Bind(const std::string& resourceName, const ShaderImageDesc& imageDesc) {
    auto& device = RenderBackend::GetInstance().GetDevice();
    if (!m_reflectionDatas.contains(resourceName)) {
        WIND_CORE_ERROR("Fail to find shader resource {}", resourceName);
    }
    vk::DescriptorImageInfo imageInfo;
    imageInfo.setImageLayout(ImageUsageToImageLayout(imageDesc.usage))
        .setImageView(imageDesc.image->GetNativeView(ImageView::NATIVE))
        .setSampler(imageDesc.sampler->GetNativeHandle());

    auto                   bindData = m_reflectionDatas[resourceName];
    vk::WriteDescriptorSet writer;
    writer.setDescriptorType(bindData.descriptorType)
        .setDescriptorCount(bindData.count)
        .setDstBinding(bindData.binding)
        .setImageInfo(imageInfo)
        .setDstSet(m_descriptorSets[bindData.set]);

    device.updateDescriptorSets(1, &writer, 0, nullptr);
}

void GraphicsShader::Bind(const std::string& resourceName, const std::vector<Image>& textureArray) {
    auto& device = RenderBackend::GetInstance().GetDevice();
    if (!m_reflectionDatas.contains(resourceName)) {
        WIND_CORE_ERROR("Fail to find shader resource {}", resourceName);
    }
    auto                                 bindData = m_reflectionDatas[resourceName];
    vk::WriteDescriptorSet               writer;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    
    for (const auto& image : textureArray) {
        auto& info = imageInfos.emplace_back();
        info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(image.GetNativeView(ImageView::NATIVE))
            .setSampler(nullptr);
    }

    writer.setDescriptorType(vk::DescriptorType::eSampledImage)
        .setDescriptorCount(imageInfos.size())
        .setImageInfo(imageInfos)
        .setDstBinding(bindData.binding)
        .setDstSet(m_descriptorSets[bindData.set]);

    device.updateDescriptorSets(1, &writer, 0, nullptr);
}

void GraphicsShader::Bind(const std::string& resourceName, const ShaderBufferDesc& bufferDesc) {
    auto& device = RenderBackend::GetInstance().GetDevice();
    if (!m_reflectionDatas.contains(resourceName)) {
        WIND_CORE_ERROR("Fail to find shader resource {}", resourceName);
    }
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setBuffer(bufferDesc.buffer->GetNativeHandle())
        .setOffset(bufferDesc.offset)
        .setRange(bufferDesc.range);
    auto                   bindData = m_reflectionDatas[resourceName];
    vk::WriteDescriptorSet writer;
    writer.setDescriptorType(bindData.descriptorType)
        .setDescriptorCount(bindData.count)
        .setDstBinding(bindData.binding)
        .setBufferInfo(bufferInfo)
        .setDstSet(m_descriptorSets[bindData.set]);

    device.updateDescriptorSets(1, &writer, 0, nullptr);
}

void GraphicsShader::Bind(const std::string& resourceName, std::shared_ptr<Sampler> sampler) {
    auto& device = RenderBackend::GetInstance().GetDevice();
    if (!m_reflectionDatas.contains(resourceName)) {
        WIND_CORE_ERROR("Fail to find shader resource {}", resourceName);
    }
    vk::DescriptorImageInfo imageInfo;
    imageInfo.setSampler(sampler->GetNativeHandle());

    auto                   bindData = m_reflectionDatas[resourceName];
    vk::WriteDescriptorSet writer;
    writer.setDescriptorType(vk::DescriptorType::eSampler)
        .setImageInfo(imageInfo)
        .setDescriptorCount(bindData.count)
        .setDstBinding(bindData.binding)
        .setDstSet(m_descriptorSets[bindData.set]);
    device.updateDescriptorSets(1, &writer, 0, nullptr);
}

std::shared_ptr<GraphicsShader>
ShaderFactory::CreateGraphicsShader(const std::string& vertexFilePath,
                                    const std::string& fragFilePath) {
    return std::make_shared<GraphicsShader>(vertexFilePath, fragFilePath);
}

} // namespace wind