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

void GraphicsShader::FinishShaderBinding() {
    auto&                               device = RenderBackend::GetInstance().GetDevice();
    std::vector<vk::WriteDescriptorSet> writeDescriptorVec;

    std::vector<vk::DescriptorBufferInfo> bufferInfos(m_reflectionDatas.size());
    std::vector<vk::DescriptorImageInfo>  imagesInfos(m_reflectionDatas.size());
    size_t                                bufferIdx = 0, imageIdx = 0;

    for (const auto& [name, metaData] : m_reflectionDatas) {
        vk::WriteDescriptorSet writer;

        if (metaData.descriptorType == vk::DescriptorType::eUniformBuffer) {
            auto& shaderBufferDesc = m_bufferShaderResource[name];

            if (shaderBufferDesc.buffer == nullptr) {
                WIND_CORE_ERROR("Fail to find shader buffer resource which is {}", name);
            }

            bufferInfos[bufferIdx]
                .setBuffer(shaderBufferDesc.buffer->GetNativeHandle())
                .setOffset(shaderBufferDesc.offset)
                .setRange(shaderBufferDesc.range);

            writer.setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDstBinding(metaData.binding)
                .setDescriptorCount(metaData.count)
                .setBufferInfo(bufferInfos[bufferIdx])
                .setDstSet(m_descriptorSets[metaData.set]);
            ++bufferIdx;
        }

        if (metaData.descriptorType == vk::DescriptorType::eCombinedImageSampler) {
            auto& shaderImageDesc = m_imageShaderResource[name];

            if (shaderImageDesc.image == nullptr) {
                WIND_CORE_ERROR("Fail to find shader image resource which is {}", name);
            }

            imagesInfos[imageIdx]
                .setImageLayout(ImageUsageToImageLayout(shaderImageDesc.usage))
                .setImageView(shaderImageDesc.image->GetNativeView(ImageView::NATIVE))
                .setSampler(shaderImageDesc.sampler->GetNativeHandle());

            writer.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setImageInfo(imagesInfos[imageIdx])
                .setDescriptorCount(metaData.count)
                .setDstBinding(metaData.binding)
                .setDstSet(m_descriptorSets[metaData.set]);
            ++imageIdx;
        }

        writeDescriptorVec.push_back(writer);
    }
    device.updateDescriptorSets(writeDescriptorVec.size(), writeDescriptorVec.data(), 0, nullptr);
}

void GraphicsShader::CollectSpirvMetaData(std::vector<uint32_t> spivrBinary,
                                          vk::ShaderStageFlags  shaderFlags) {
    const auto& device = RenderBackend::GetInstance().GetDevice();

    spirv_cross::CompilerGLSL    compiler(std::move(spivrBinary));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    for (auto& resource : resources.uniform_buffers) {
        // check our uniform buffer
        if (m_reflectionDatas.find(resource.name) == m_reflectionDatas.end()) {
            std::string_view resourceName = resource.name;
            uint32_t set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            const spirv_cross::SPIRType& type          = compiler.get_type(resource.type_id);
            uint32_t                     size          = compiler.get_declared_struct_size(type);
            uint32_t                     typeArraySize = type.array.size();
            uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
            BindMetaData metaData{set, binding, count, vk::DescriptorType::eUniformBuffer,
                                  shaderFlags};
            m_reflectionDatas[resource.name] = metaData;
        } else {
            m_reflectionDatas[resource.name].shaderStageFlag |= shaderFlags;
        }
    }

    for (auto& resource : resources.sampled_images) {
        if (m_reflectionDatas.find(resource.name) == m_reflectionDatas.end()) {
            std::string_view resourceName = resource.name;
            uint32_t set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            const spirv_cross::SPIRType& type          = compiler.get_type(resource.type_id);
            uint32_t                     typeArraySize = type.array.size();
            uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
            BindMetaData metaData{set, binding, count, vk::DescriptorType::eCombinedImageSampler,
                                  shaderFlags};
            m_reflectionDatas[resource.name] = metaData;
        } else {
            m_reflectionDatas[resource.name].shaderStageFlag |= shaderFlags;
        }
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

GraphicsShader& GraphicsShader::SetShaderResource(std::string_view        resourceName,
                                                  const ShaderBufferDesc& bufferDesc) {
    m_bufferShaderResource[std::string(resourceName)] = bufferDesc;
    return *this;
}

GraphicsShader& GraphicsShader::SetShaderResource(std::string_view       resourceName,
                                                  const ShaderImageDesc& imageDesc) {
    m_imageShaderResource[std::string(resourceName)] = imageDesc;
    return *this;
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

void GraphicsShader::Bind(const std::string resoueceName, std::shared_ptr<Image> image) {
    auto& shaderImageDesc = m_imageShaderResource[resoueceName];
    shaderImageDesc.image = image;
}

std::shared_ptr<GraphicsShader>
ShaderFactory::CreateGraphicsShader(const std::string& vertexFilePath,
                                    const std::string& fragFilePath) {
    return std::make_shared<GraphicsShader>(vertexFilePath, fragFilePath);
}

} // namespace wind