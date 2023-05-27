#pragma once

#include <functional>
#include <memory>

#include "Runtime/Render/RHI/Backend.h"

namespace wind {
class RenderGraph;
enum class AttachmentType : uint8_t { COLOR = 0, DEPTH, INPUT };

struct PassNode {
    vk::RenderPass        renderPassHandle;
    vk::Pipeline          pipeline;
    vk::PipelineLayout    pipelineLayout;
    vk::PipelineBindPoint pipelineType = {};
    vk::Rect2D            renderArea   = {};
    vk::ClearValue        clearValues;
};

struct ResourceNode {
    uint32_t                             presentImageIndex;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    std::vector<vk::DescriptorSet>       descriptorSets;
    std::vector<vk::Framebuffer>         framebuffers;
    std::vector<Image>                   colorAttachments;
    std::vector<Image>                   depthAttachments;
};

class RenderPassBase {
public:
    virtual ~RenderPassBase();
    RenderPassBase(std::string_view name, RenderGraph& renderGraph)
        : passName(name), renderBackend(RenderBackend::GetInstance()), renderGraph(renderGraph) {}

    [[nodiscard]] auto& Name() const { return passName; }
    virtual void        Setup() = 0;
    virtual void        Exec()  = 0;

protected:
    std::string    passName;
    PassNode       passNode;
    ResourceNode   resourceNode;
    RenderBackend& renderBackend;
    RenderGraph&   renderGraph;
};

} // namespace wind