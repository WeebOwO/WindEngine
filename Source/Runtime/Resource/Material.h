#pragma once

#include <memory>
#include <unordered_map>

#include "Runtime/Render/RHI/CommandBuffer.h"
#include "Runtime/Render/RHI/Shader.h"

namespace wind {

class Material {
public:
    static void LoadMaterial(std::string_view filePath);
    static void AddMaterial(std::string_view materialName);

    static auto GetMaterialByName(std::string_view materialName) {
        return m_materialCache[std::string(materialName)];
    }

    void Bind(CommandBuffer commandBuffer);

private:
    std::string m_name;
    std::shared_ptr<GraphicsShader> m_shader;
    static std::unordered_map<std::string, std::shared_ptr<Material>> m_materialCache;
};

} // namespace wind