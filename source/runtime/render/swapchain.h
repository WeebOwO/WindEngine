#pragma once

#include <cstdint>
#include <tuple>
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
    SwapChain(uint32_t width, uint32_t height, uint32_t maxFrameInFlight);
    ~SwapChain();
    void             QueryInfo(uint32_t width, uint32_t height, uint32_t maxFrameInFlight);
    void             CreateImageView();
    void             CreateFrameBuffers();
    std::tuple<uint32_t, uint32_t> GetViewPortSize() {return {m_currentWidth, m_currentHeight};}
    vk::SwapchainKHR swapchain;

    std::vector<vk::Image>     images;
    std::vector<vk::ImageView> imageViews;

    SwapChainInfo swapchainInfo;

private:
    uint32_t m_currentWidth, m_currentHeight;
};

} // namespace wind