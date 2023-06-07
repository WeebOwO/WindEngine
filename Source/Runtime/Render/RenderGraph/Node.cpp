#include "Node.h"

namespace wind {
void PassNode::Init(const std::string& passName, std::span<std::string_view> inRoureces,
                    std::span<std::string_view> outputResources) {
    // add this to mark different
    this->passName = passName;
    if (!inRoureces.empty()) { 
        inRefCnt = inRoureces.size();
        dependencyResources = {inRoureces.begin(), inRoureces.end()}; 
    }
    if (!outputResources.empty()) {
        outRefcnt = outputResources.size();
        this->outputResources = {outputResources.begin(), outputResources.end()};
    }
}
} // namespace wind