#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/RenderPassBase.h"  

namespace wind {
class Image;
class RenderPassBase;

class RenderGraph {
public:
    void AddRenderPass(std::unique_ptr<wind::RenderPassBase> renderPass);
    void DeclareAttachmentOutput(std::string_view passName, std::shared_ptr<Image> attachment);
    void DeclareUniformBuffer(std::string_view passName);
    void Exec();
    void Setup();

private:
    std::list<std::unique_ptr<RenderPassBase>> m_renderPassList;
    std::unordered_map<std::string, std::unique_ptr<RenderPassBase>> m_renderPassMap;
    std::unordered_map<std::string, std::shared_ptr<Image>> m_graphAttachments;
};
} // namespace wind