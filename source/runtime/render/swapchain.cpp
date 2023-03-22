#include "swapchain.h"

#include "context.h"

namespace wind {
    SwapChain::SwapChain(uint32_t width, uint32_t height) {
        QueryInfo(width, height);

        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.setClipped(VK_TRUE)
                  .setImageArrayLayers(1)
                  .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                  .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                  .setSurface(RenderContext::GetInstace().surface)
                  .setImageColorSpace(swapchainInfo.surfaceFormat.colorSpace)
                  .setImageFormat(swapchainInfo.surfaceFormat.format)
                  .setImageExtent(swapchainInfo.imageExtent)
                  .setMinImageCount(swapchainInfo.imageCount)
                  .setPreTransform(swapchainInfo.transform)
                  .setPresentMode(swapchainInfo.presentMode);

        auto queueIndices = RenderContext::GetInstace().queueIndices;
        if(queueIndices.graphicsQueueIndex == queueIndices.presentQueueIndex) {
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        } else{
            std::array indices = {queueIndices.graphicsQueueIndex.value(), queueIndices.presentQueueIndex.value()};
            createInfo.setQueueFamilyIndices(indices)
                      .setImageSharingMode(vk::SharingMode::eConcurrent);
        }

        swapchain = RenderContext::GetInstace().device.createSwapchainKHR(createInfo);
    }

    void SwapChain::QueryInfo(uint32_t width, uint32_t height) {
        auto& phyDevice  = RenderContext::GetInstace().physicalDevice;
        auto& surface = RenderContext::GetInstace().surface;
        auto formats = phyDevice.getSurfaceFormatsKHR(surface);
        
        swapchainInfo.surfaceFormat = formats[0];
        for (const auto& availableFormat : formats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                swapchainInfo.surfaceFormat = availableFormat;
                break;
            }
        }

        auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
        swapchainInfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
        
        swapchainInfo.imageExtent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        swapchainInfo.imageExtent.height = std::clamp<uint32_t>(width, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        swapchainInfo.transform = capabilities.currentTransform;

        // present mode select
        auto presents = phyDevice.getSurfacePresentModesKHR(surface);
        swapchainInfo.presentMode = vk::PresentModeKHR::eFifo;
        for(const auto& present : presents) {
            if(present == vk::PresentModeKHR::eMailbox) {
                swapchainInfo.presentMode = present;
                std::cout << "using mailbox mode to present!\n";
                break;
            }
        }
    }
}