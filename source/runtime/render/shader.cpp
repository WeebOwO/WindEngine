#include "runtime/render/shader.h"

#include "runtime/render/context.h"
#include "runtime/render/utils.h"

namespace wind {
    Shader::Shader(const std::vector<char>& vertexSource, const std::vector<char>& fragSource) {
        m_vertexShaderModule = utils::CreateShaderModule(vertexSource);
        m_fragmentShaderModule = utils::CreateShaderModule(fragSource);
    }
    Shader::~Shader() {
        auto& device = RenderContext::GetInstace().device;
        device.destroyShaderModule(m_vertexShaderModule);
        device.destroyShaderModule(m_fragmentShaderModule);
    }
}