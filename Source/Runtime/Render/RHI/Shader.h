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

class GraphicsShader {
public:
    struct ShaderResource {
        std::variant<std::shared_ptr<Buffer>, std::shared_ptr<Image>> resource;
    };
    
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
    [[nodiscard]] auto& GetDescriptorSetLayouts() const { return m_descriptorSetLayout; }

    void Bind(const std::string resourceName, uint8_t* cpudata);
    void Bind(const std::string resoueceName, Sampler sampler, uint8_t* cpudata);
    
private:
    void GenerateVulkanDescriptorSetLayout();
    void CollectSpirvMetaData(std::vector<uint32_t> spivrBinary, vk::ShaderStageFlags shaderFlags);
    vk::ShaderModule                                  m_vertexShader;
    vk::ShaderModule                                  m_fragShader;

    std::unordered_map<std::string, BindMetaData>     m_reflectionDatas;
    
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorSet       m_descriptorSet;
};  

class ShaderFactory {
public:
    static std::shared_ptr<GraphicsShader>
    CreateGraphicsShader(const std::string& name, const std::string& vertexFilePath = "",
                         const std::string& fragFilePath = "");

private:
    static std::unordered_map<std::string, std::shared_ptr<GraphicsShader>> m_shaderCache;
};

} // namespace wind
