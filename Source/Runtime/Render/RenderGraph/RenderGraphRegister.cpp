#include "RenderGraphRegister.h"

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
} // namespace wind