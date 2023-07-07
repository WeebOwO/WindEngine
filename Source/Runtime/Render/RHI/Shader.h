#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <variant>
#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Sampler.h"

namespace wind {
struct ShaderBase {};

struct ShaderImageDesc {
    std::shared_ptr<Image>   image;
    vk::ImageLayout          layout;
    std::shared_ptr<Sampler> sampler;
};

struct ShaderBufferDesc {
    std::shared_ptr<Buffer> buffer;
    size_t                  offset;
    size_t                  range;
};

class GraphicsShader : public ShaderBase {
public:
    struct BindMetaData {
        uint32_t             set;
        uint32_t             binding;
        uint32_t             count;
        vk::DescriptorType   descriptorType;
        vk::ShaderStageFlags shaderStageFlag;
    };

    GraphicsShader(std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath);

    ~GraphicsShader();

    [[nodiscard]] auto  GetVertexShaderModule() const { return m_vertexShader; }
    [[nodiscard]] auto  GetFragmentShaderModule() const { return m_fragShader; }
    [[nodiscard]] auto  GetShaderReflesctionData() const { return m_reflectionDatas; }
    [[nodiscard]] auto& GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
    [[nodiscard]] auto& GetDescriptorSet() {return m_descriptorSets;}

    void Bind(const std::string resourceName, uint8_t* cpudata);
    void Bind(const std::string resoueceName, Sampler sampler, uint8_t* cpudata);
    
    void FinishShaderBinding();

    GraphicsShader& SetShaderResource(std::string_view resourceName, const ShaderBufferDesc& bufferDesc);
    GraphicsShader& SetShaderResource(std::string_view resourceName, const ShaderImageDesc& imageDesc);
     
private:
    void GenerateVulkanDescriptorSetLayout();
    void CollectSpirvMetaData(std::vector<uint32_t> spivrBinary, vk::ShaderStageFlags shaderFlags);
    vk::ShaderModule m_vertexShader;
    vk::ShaderModule m_fragShader;

    std::unordered_map<std::string, BindMetaData> m_reflectionDatas;

    std::unordered_map<std::string, ShaderImageDesc>  m_imageShaderResource;
    std::unordered_map<std::string, ShaderBufferDesc> m_bufferShaderResource;

    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<vk::DescriptorSet>       m_descriptorSets;
};

class ShaderFactory {
public:
    static std::shared_ptr<GraphicsShader>
    CreateGraphicsShader(const std::string& vertexFilePath = "",
                         const std::string& fragFilePath   = "");
};

} // namespace wind
