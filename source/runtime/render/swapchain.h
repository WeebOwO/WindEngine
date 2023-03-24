#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

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
    void             CreateFrameBuffers();
    vk::SwapchainKHR swapchain;

    std::vector<vk::Image>     images;
    std::vector<vk::ImageView> imageViews;
    std::vector<vk::Framebuffer> frameBuffers;
    SwapChainInfo swapchainInfo;
private:
    uint32_t m_currentWidth, m_currentHeight;
};

} // namespace wind