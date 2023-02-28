#pragma once

#include "lve_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace lve {

class LveSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    LveSwapChain(LveDevice& deviceRef, VkExtent2D windowExtent);
    ~LveSwapChain();

    LveSwapChain(const LveSwapChain&)   = delete;
    void operator=(const LveSwapChain&) = delete;

    VkFramebuffer GetFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
    VkRenderPass  GetRenderPass() { return m_renderPass; }
    VkImageView   GetImageView(int index) { return m_swapChainImageViews[index]; }
    size_t        ImageCount() { return m_swapChainImages.size(); }
    VkFormat      GetSwapChainImageFormat() { return m_swapChainImageFormat; }
    VkExtent2D    GetSwapChainExtent() { return m_swapChainExtent; }
    uint32_t      width() { return m_swapChainExtent.width; }
    uint32_t      height() { return m_swapChainExtent.height; }

    float ExtentAspectRatio() {
        return static_cast<float>(m_swapChainExtent.width) /
               static_cast<float>(m_swapChainExtent.height);
    }
    VkFormat FindDepthFormat();

    VkResult AcquireNextImage(uint32_t* imageIndex);
    VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

private:
    void CreateSwapChain();
    void CreateImageViews();
    void CreateDepthResources();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR
    ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR
               ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkFormat   m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkRenderPass               m_renderPass;

    std::vector<VkImage>        m_depthImages;
    std::vector<VkDeviceMemory> m_depthImageMemorys;
    std::vector<VkImageView>    m_depthImageViews;
    std::vector<VkImage>        m_swapChainImages;
    std::vector<VkImageView>    m_swapChainImageViews;

    LveDevice& m_device;
    VkExtent2D m_windowExtent;

    VkSwapchainKHR m_swapChain;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence>     m_inFlightFences;
    std::vector<VkFence>     m_imagesInFlight;
    size_t                   m_currentFrame = 0;
};

} // namespace lve