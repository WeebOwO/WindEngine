#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/Descriptors.h"
#include "Runtime/Render/RHI/Frame.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Render/Window.h"

#include "Runtime/Scene/SceneView.h"

namespace wind {

struct SceneResourcePool {
    SceneResourcePool();
    
    struct CameraBuffer {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    struct ObjectBuffer {
        glm::mat4 model;
    };

    void Init();

    void UpdateCameraBuffer(Camera* camera);
    void UpdateObjectBuffer(GameObject* gameObject);

    BufferInfo GetBuffer(const std::string& bufferName);
    ImageInfo  GetImage(const std::string& ImageName);

private:
    std::unordered_map<std::string, std::shared_ptr<Buffer>> m_predefinedBuffer ;
    std::shared_ptr<Buffer>                                  m_camearaUniformBuffer;
    std::shared_ptr<Buffer>                                  m_objectUniformBuffer;
};

struct QueueIndices {
    std::optional<uint32_t> graphicsQueueIndex;
    std::optional<uint32_t> presentQueueIndex;
    std::optional<uint32_t> computeQueueIndex;

    bool IsComplete() {
        return graphicsQueueIndex.has_value() && presentQueueIndex.has_value() &&
               computeQueueIndex.has_value();
    }
};

struct BackendCreateSetting {
    Window&  window;
    uint32_t maxFrameInflight{2};
    uint32_t maxStageBufferSize{64 * 1024 * 1024};
};

class RenderBackend {
public:
    PERMIT_COPY(RenderBackend);
    PERMIT_MOVE(RenderBackend);

    RenderBackend(const BackendCreateSetting& setting) noexcept;
    ~RenderBackend();

    static void Init(const BackendCreateSetting& setting);

    static RenderBackend& GetInstance() {
        assert(s_instance);
        return *s_instance;
    }

    void RecreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight);
    void StartFrame() { m_virtualFrames.StartFrame(); }
    void EndFrame() { m_virtualFrames.EndFrame(); }

    CommandBuffer BeginSingleTimeCommand();
    void          SubmitSingleTimeCommand(vk::CommandBuffer cmdBuffer);

    void InitVirtualFrame() {
        m_virtualFrames.Init(m_createSetting.maxFrameInflight, m_createSetting.maxStageBufferSize);
    }

    [[nodiscard]] const auto& GetCommandPool() const noexcept { return m_coomandPool; }
    [[nodiscard]] const auto& GetDevice() const noexcept { return m_device; }
    [[nodiscard]] const auto& GetPhyDevice() const noexcept { return m_physicalDevice; }
    [[nodiscard]] const auto& GetPresentQueue() const noexcept { return m_presentQueue; }
    [[nodiscard]] const auto& GetGraphicsQueue() const noexcept { return m_graphicsQueue; }
    [[nodiscard]] const auto& GetVkInstance() const noexcept { return m_vkInstance; }

    // get swapchain related things
    [[nodiscard]] const auto& GetSurface() const noexcept { return m_surface; }
    [[nodiscard]] const auto& GetSurfaceExtent() const noexcept { return m_surfaceExtent; }
    [[nodiscard]] const auto& GetSwapChainSurfaceFormat() const noexcept {
        return m_surfaceFormat.format;
    }
    [[nodiscard]] const auto& GetSwapChain() const noexcept { return m_swapchain; }

    [[nodiscard]] const auto& GetImageAvailableSemaphore() const noexcept {
        return m_imageAvailableSemaphore;
    }
    [[nodiscard]] const auto& GetRenderingFinishedSemaphore() const noexcept {
        return m_renderingFinishedSemaphore;
    }
    [[nodiscard]] const auto& GetAllocator() const noexcept { return m_allocator; }
    [[nodiscard]] const auto& GetSwapchain() const noexcept { return m_swapchain; }

    [[nodiscard]] const auto GetSwapchainImageUsage(size_t index) const noexcept {
        return m_swapchainImageUsages[index];
    }
    [[nodiscard]] const auto  GetPresentImageCnt() const { return m_presentImageCnt; }
    [[nodiscard]] const auto& AcquireSwapchainImage(size_t index, ImageUsage::Bits usage) noexcept {
        m_swapchainImageUsages[index] = usage;
        return m_swapchainImages[index];
    }
    [[nodiscard]] auto& GetCurrentSwapChainImage() const {
        return m_swapchainImages[m_virtualFrames.GetPresentImageIndex()];
    }
    [[nodiscard]] const auto     IsRenderingEnabled() const { return m_renderingEnabled; }
    [[nodiscard]] constexpr auto GetAPIVersion() { return VK_MAKE_VERSION(1, 0, 3); }
    [[nodiscard]] const auto     GetCurrentImageIndex() {
        return m_virtualFrames.GetPresentImageIndex();
    }
    [[nodiscard]] auto&       GetCurrentFrame() const { return m_virtualFrames.GetCurrentFrame(); }
    [[nodiscard]] const auto& GetDescriptorLayoutCache() const { return m_descriptorLayoutCache; }
    [[nodiscard]] const auto& GetDescriptorAllocator() const { return m_descriptorAllocator; }
    [[nodiscard]] auto&       GetStagingBuffer() {
        return m_virtualFrames.GetCurrentFrame().StagingBuffer;
    }
    [[nodiscard]] auto  GetMaxFrameInFlight() { return m_createSetting.maxFrameInflight; }
    [[nodiscard]] auto& GetCurrentResourcePool() { return m_sceneResourcePools[m_virtualFrames.GetPresentImageIndex()]; }
    [[nodiscard]] auto& GetSceneResourcesPools() {return m_sceneResourcePools;}
    
private:
    std::vector<const char*> GetRequiredExtensions();
    void                     CreateInstance();
    void                     PickupPhysicalDevice();
    void                     CreateDevice();
    void                     CreateSuface();
    void                     QueryQueueFamilyIndices();
    void                     QuerySurfaceProperty();
    void                     GetQueue();
    void                     CreateCmdPool();
    void                     CreateSyncObeject();
    void                     CreateVmaAllocator();
    void                     CreateDescriptorCacheAndAllocator();
    void                     CreateSceneResourcePools();

    BackendCreateSetting m_createSetting;

    vk::Instance                 m_vkInstance;
    vk::PhysicalDevice           m_physicalDevice;
    vk::PhysicalDeviceProperties m_physicalDeviceProperties;
    vk::Device                   m_device;

    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;
    vk::Queue m_computeQueue;

    QueueIndices m_queueIndices;

    vk::CommandPool m_coomandPool;

    vk::SurfaceKHR       m_surface;
    vk::SwapchainKHR     m_swapchain;
    vk::Extent2D         m_surfaceExtent;
    vk::SurfaceFormatKHR m_surfaceFormat;
    vk::PresentModeKHR   m_surfacePresentMode;

    std::vector<Image>            m_swapchainImages;
    std::vector<ImageUsage::Bits> m_swapchainImageUsages;

    uint32_t m_imageCount;

    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderingFinishedSemaphore;
    vk::Fence     m_immediateFence;

    VmaAllocator         m_allocator;
    VirtualFrameProvider m_virtualFrames;

    std::shared_ptr<DescriptorAllocator>   m_descriptorAllocator;
    std::shared_ptr<DescriptorLayoutCache> m_descriptorLayoutCache;

    std::vector<SceneResourcePool>        m_sceneResourcePools;
    uint32_t                              m_presentImageCnt;
    bool                                  m_renderingEnabled{true};
    static std::unique_ptr<RenderBackend> s_instance;
};

}; // namespace wind