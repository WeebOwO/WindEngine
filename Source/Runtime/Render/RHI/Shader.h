#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

namespace wind {

class GraphicsShader {
public:
    struct BindMetaData {
        uint32_t                          binding;
        vk::DescriptorType                descriptorType;
        std::vector<vk::ShaderStageFlags> shaderStageFlags;
    };

    GraphicsShader(std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath);

    ~GraphicsShader();

    [[nodiscard]] auto  GetVertexShaderModule() const { return m_vertexShader; }
    [[nodiscard]] auto  GetFragmentShaderModule() const { return m_fragShader; }
    [[nodiscard]] auto  GetShaderReflesctionData() const { return m_reflectionDatas; }
    [[nodiscard]] auto& GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }
    [[nodiscard]] auto& GetDescriptorBindingGroup() { return m_setGroups; }

private:
    void GenerateVulkanDescriptorSetLayout();
    void CollectSpirvMetaData(std::vector<uint32_t> spivrBinary, vk::ShaderStageFlags shaderFlags);
    vk::ShaderModule                                  m_vertexShader;
    vk::ShaderModule                                  m_fragShader;
    std::unordered_map<std::string, BindMetaData>     m_reflectionDatas;
    std::unordered_map<int, std::vector<std::string>> m_setGroups;

    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<vk::DescriptorSet>       m_descriptorSet;
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
