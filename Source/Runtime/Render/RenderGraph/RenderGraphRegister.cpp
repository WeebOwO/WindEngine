#include "RenderGraphRegister.h"

#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
void RenderGraphRegister::RegisterPassResouce(const std::string& passName,
                                              const std::string& resourceName,
                                              DataRelation       relation) {
    if (relation == DataRelation::Read) {
        m_passReadResources[passName].push_back(resourceName);
    } else {
        m_passWriteResources[passName].push_back(resourceName);
    }
}

void RenderGraphRegister::RegisterResource(const std::string& resoursename, ResourceNode* resource) {
    m_resouceLookupTable[resoursename] = resource;
}

bool RenderGraphRegister::Contains(const std::string &resourceName) {
    return m_resouceLookupTable.find(resourceName) != m_resouceLookupTable.end();
}
} // namespace wind