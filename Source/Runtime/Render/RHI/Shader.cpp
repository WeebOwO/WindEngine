#include "runtime/render/rhi/shader.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <unordered_map>

#include "Runtime/Base/Io.h"
#include "Runtime/Render/Rhi/Backend.h"

namespace wind {

void GraphicsShader::CollectSpirvMetaData(std::vector<uint32_t> spivrBinary) {
    const auto& device = RenderBackend::GetInstance().GetDevice();
    spirv_cross::CompilerGLSL    compiler(std::move(spivrBinary));
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    
    for (auto& resource : resources.uniform_buffers) {
      
        uint32_t set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        
        m_shaderMetaDatas.push_back({set, binding, vk::DescriptorType::eUniformBuffer});

        WIND_INFO("Here is uniform buffer, set is {}, binding is {}", set, binding);
    }   

    
}

GraphicsShader::~GraphicsShader() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.destroyShaderModule(m_vertexShader);
    device.destroyShaderModule(m_fragShader);
}

GraphicsShader::GraphicsShader(std::string_view vertexShaderfilePath, std::string_view fragmentShaderFilePath) {
    auto& device = RenderBackend::GetInstance().GetDevice();

    auto spirvVertexBinary = io::ReadSpirvBinaryFile(vertexShaderfilePath);
    auto spirvFragBinary   = io::ReadSpirvBinaryFile(fragmentShaderFilePath);

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setPCode(spirvVertexBinary.data())
        .setCodeSize(spirvVertexBinary.size() * sizeof(uint32_t));
    m_vertexShader = device.createShaderModule(createInfo);

    createInfo.setPCode(spirvFragBinary.data())
        .setCodeSize(spirvFragBinary.size() * sizeof(uint32_t));

    m_fragShader = device.createShaderModule(createInfo);
}

} // namespace wind