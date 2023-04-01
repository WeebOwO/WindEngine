#pragma once

#include <vulkan/vulkan.hpp>

namespace wind {
class Shader {
public:
    Shader(const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
    ~Shader();

    [[nodiscard]] auto vertexShaderModule() const { return m_vertexShaderModule; };
    [[nodiscard]] auto fragmentShaderModule() const { return m_fragmentShaderModule; }

private:
    vk::ShaderModule m_vertexShaderModule;
    vk::ShaderModule m_fragmentShaderModule;
};
} // namespace wind