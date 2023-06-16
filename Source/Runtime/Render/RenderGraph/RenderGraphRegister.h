#pragma once

#include "Runtime/Base/Macro.h"
#include <string>
#include <span>
#include <unordered_map>

namespace wind {
class ResourceNode;

enum class DataRelation : uint8_t {
    Read = 0,
    Write
};

class RenderGraphRegister {
public:
    [[nodiscard]] auto GetResource(std::string_view resourceName) {
        if(m_resouceLookupTable.find(std::string(resourceName)) == m_resouceLookupTable.end()) {
            WIND_CORE_WARN("Fail to find {}", resourceName);
        }
        return m_resouceLookupTable[std::string(resourceName)];
    }
    void RegisterPassResouce(const std::string& passName, const std::string& resourceName, DataRelation relation);

    void RegisterResource(const std::string& resoursename, ResourceNode* resource);
    void DecalareOutput(std::span<std::string> outputs);
    void SetupDependency(std::span<std::string> dependencies);

    void UnRegisterAll();
private:
    std::pmr::unordered_map<std::string, ResourceNode*> m_resouceLookupTable;
    std::pmr::unordered_map<std::string, std::list<std::string>> m_passReadResources;
    std::pmr::unordered_map<std::string, std::list<std::string>> m_passWriteResources;
};
} // namespace wind