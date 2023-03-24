#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "runtime/render/context.h"

namespace wind {

struct SwapChainInfo {
    vk::Extent2D                    imageExtent;
    uint32_t                        imageCount;
    vk::SurfaceFormatKHR            surfaceFormat;
    vk::PresentModeKHR              presentMode;
    vk::SurfaceTransformFlagBitsKHR transform;
};

class SwapChain {
public:
    SwapChain(uint32_t width, uint32_t height);
    ~SwapChain();
    void             QueryInfo(uint32_t width, uint32_t height);
    void             CreateImageView();
    vk::SwapchainKHR swapchain;

    std::vector<vk::Image>     images;
    std::vector<vk::ImageView> imageViews;

    SwapChainInfo swapchainInfo;
};

} // namespace wind