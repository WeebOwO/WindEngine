#pragma once

#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include <string>

#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
enum class DataRelation : uint8_t {
    Read = 0,
    Write
};

class RenderGraphRegister {
public:
    [[nodiscard]] auto GetResource(std::string_view resourceName) {
        return m_resouceLookupTable[std::string(resourceName)];
    }
    void RegisterPassResouce(const std::string& passName, const std::string& resourceName, DataRelation relation);

    void RegisterResource(const std::string& resoursename, ResourceNode* resource);
    void DecalareOutput(std::span<std::string> outputs);
    void SetupDependency(std::span<std::string> dependencies);

    void UnRegisterAll();
private:
    std::unordered_map<std::string, ResourceNode*> m_resouceLookupTable;
    std::unordered_map<std::string, std::list<std::string>> m_passReadResources;
    std::unordered_map<std::string, std::list<std::string>> m_passWriteResources;
};
} // namespace wind