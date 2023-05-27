#pragma once

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/Rhi/CommandBuffer.h"

namespace wind {

struct VirtualFrame {
    CommandBuffer Commands{vk::CommandBuffer{}};
    vk::Fence     CommandQueueFence;
};

class VirtualFrameProvider {
public:
    void Init(size_t frameCount, size_t stageBufferSize);
    void Destroy();

    void                              StartFrame();
    VirtualFrame&                     GetCurrentFrame();
    VirtualFrame&                     GetNextFrame();
    [[nodiscard]] const VirtualFrame& GetCurrentFrame() const;
    [[nodiscard]] const VirtualFrame& GetNextFrame() const;
    [[nodiscard]] uint32_t            GetPresentImageIndex() const;
    [[nodiscard]] size_t              GetFrameCount() const;
    void                              EndFrame();

private:
    std::vector<VirtualFrame> m_virtualFrames;
    uint32_t                  m_presentImageIndex = 0;
    bool                      m_isFrameRunning    = false;
    size_t                    m_currentFrame      = 0;
};
} // namespace wind