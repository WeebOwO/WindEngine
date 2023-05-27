#include "RenderPassBase.h"

namespace wind {
RenderPassBase::~RenderPassBase() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.waitIdle();
    device.destroyRenderPass(passNode.renderPassHandle);
    device.destroyPipelineLayout(passNode.pipelineLayout);
    device.destroyPipeline(passNode.pipeline);
    for (size_t i = 0; i < resourceNode.framebuffers.size(); ++i) {
        device.destroyFramebuffer(resourceNode.framebuffers[i]);
    }
}

} // namespace wind