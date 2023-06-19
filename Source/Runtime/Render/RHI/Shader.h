#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

namespace wind {

class GraphicsShader {
public:
    struct ShaderMetaData {
        uint32_t           set;
        uint32_t           binding;
        vk::DescriptorType descriptorType;
    };

    GraphicsShader(std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath);

    ~GraphicsShader();
    [[nodiscard]] auto GetVertexShaderModule() const { return m_vertexShader; }
    [[nodiscard]] auto GetFragmentShaderModule() const { return m_fragShader; }

private:
    void                        CollectSpirvMetaData(std::vector<uint32_t> spivrBinary);
    vk::ShaderModule            m_vertexShader;
    vk::ShaderModule            m_fragShader;
    std::vector<ShaderMetaData> m_shaderMetaDatas;

    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<vk::DescriptorSet> m_descriptorSets;
};

class ShaderFactory {
public:
    static std::shared_ptr<GraphicsShader> CreateGraphicsShader(const std::string& name,
                                                                const std::string& vertexFilePath,
                                                                const std::string& fragFilePath);

private:
    static std::unordered_map<std::string, std::shared_ptr<GraphicsShader>> m_shaderCache;
};

} // namespace wind
