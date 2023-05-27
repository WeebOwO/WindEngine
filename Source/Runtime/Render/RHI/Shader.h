#pragma once

#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace wind {

class GraphicsShader {
public:
    GraphicsShader(
        std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath,
        vk::VertexInputBindingDescription                       inputBinding,
        const std::vector<vk::VertexInputAttributeDescription>& inputAttributeDescription);

    ~GraphicsShader();
    [[nodiscard]] auto GetVertexShaderModule() const { return m_vertexShader; }
    [[nodiscard]] auto GetFragmentShaderModule() const { return m_fragShader; }

    [[nodiscard]] auto GetVertexInputBindingDescription() const {
        return m_inputBindingDescription;
    }
    
    [[nodiscard]] auto GetVertexInputAttributeDescriptions() const {
        return m_inputAttributeDescription;
    }

    [[nodiscard]] auto GenerateDescriptorSetLayouts() const {
        return m_descriptorSetLayoutMaps;
    }

private:
    void                              CollectSpirvMetaData(std::vector<uint32_t> spivrBinary);
    vk::ShaderModule                  m_vertexShader;
    vk::ShaderModule                  m_fragShader;
    vk::VertexInputBindingDescription m_inputBindingDescription;
    std::vector<vk::VertexInputAttributeDescription>      m_inputAttributeDescription;
    std::unordered_map<uint32_t, vk::DescriptorSetLayout> m_descriptorSetLayoutMaps;
};
} // namespace wind