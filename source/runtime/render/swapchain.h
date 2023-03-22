#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "runtime/render/context.h"

namespace wind {

struct SwapChainInfo {
    vk::Extent2D         imageExtent;
    uint32_t             imageCount;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::PresentModeKHR   presentMode;
    vk::SurfaceTransformFlagBitsKHR transform;
};

struct SwapChain {
    SwapChain(uint32_t width, uint32_t height);

    void QueryInfo(uint32_t width, uint32_t height);

    vk::SwapchainKHR swapchain;
    
    SwapChainInfo swapchainInfo;
};

} // namespace wind