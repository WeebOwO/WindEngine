#include "Backend.h"

#include <functional>
#include <unordered_set>
#include <vector>

#include <vk_mem_alloc.h>

#include "Runtime/Base/Macro.h"
#include "Runtime/Render/RHI/CommandBuffer.h"

static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

namespace wind {
std::unique_ptr<RenderBackend> RenderBackend::s_instance = nullptr;

template <typename T, typename U>
void RemoveNosupportedElems(std::vector<T>& elems, const std::vector<U>& supportedElems,
                            std::function<bool(const T&, const U&)> eq) {
    int i = 0;
    while (i < elems.size()) {
        if (std::find_if(supportedElems.begin(), supportedElems.end(), [&](const U& elem) {
                return eq(elems[i], elem);
            }) == supportedElems.end()) {
            elems.erase(elems.begin() + i);
        } else {
            i++;
        }
    }
}

RenderBackend::RenderBackend(const BackendCreateSetting& setting) noexcept
    : m_createSetting(setting), m_imageCount(setting.maxFrameInflight) {
    CreateInstance();
    PickupPhysicalDevice();
    CreateSuface();
    QueryQueueFamilyIndices();
    QuerySurfaceProperty();
    CreateDevice();
    CreateSyncObeject();
    CreateVmaAllocator();
    GetQueue();
    CreateCmdPool();
    CreateDescriptorCacheAndAllocator();
}

void RenderBackend::Init(const BackendCreateSetting& setting) {
    WIND_CORE_INFO("Render context init");
    s_instance = std::make_unique<RenderBackend>(setting);
    s_instance->InitVirtualFrame();
    s_instance->RecreateSwapchain(setting.window.width(), setting.window.height());
}

void RenderBackend::CreateInstance() {
    vk::InstanceCreateInfo createinfo{};
    vk::ApplicationInfo    appInfo{};

    appInfo.setApiVersion(VK_MAKE_VERSION(1, 0, 3));
    createinfo.setPApplicationInfo(&appInfo);

    RemoveNosupportedElems<const char*, vk::LayerProperties>(
        layers, vk::enumerateInstanceLayerProperties(),
        [](const char* e1, const vk::LayerProperties& e2) {
            return std::strcmp(e1, e2.layerName) == 0;
        });

    auto extensions = GetRequiredExtensions();
    createinfo.setPEnabledLayerNames(layers).setPEnabledExtensionNames(extensions);

    m_vkInstance = vk::createInstance(createinfo);
}

RenderBackend::~RenderBackend() {
    m_device.waitIdle();
    m_virtualFrames.Destroy();
    m_swapchainImages.clear();
    m_descriptorAllocator->CleanUp();

    m_device.destroySemaphore(m_renderingFinishedSemaphore);
    m_device.destroySemaphore(m_imageAvailableSemaphore);
    m_device.destroyFence(m_immediateFence);
    m_device.destroyCommandPool(m_coomandPool);
    m_device.destroySwapchainKHR(m_swapchain);

    vmaDestroyAllocator(m_allocator);
    m_device.destroy();

    m_vkInstance.destroySurfaceKHR(m_surface);
    m_vkInstance.destroy();
}

void RenderBackend::PickupPhysicalDevice() {
    auto physicalDevices = m_vkInstance.enumeratePhysicalDevices();
    // not good, but enough for me
    m_physicalDevice           = physicalDevices.front();
    m_physicalDeviceProperties = m_physicalDevice.getProperties();
    WIND_CORE_INFO(m_physicalDevice.getProperties().deviceName);
}

void RenderBackend::CreateDevice() {
    std::array           extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    vk::DeviceCreateInfo createInfo;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::unordered_set<uint32_t> uniqueQueueIndices{m_queueIndices.graphicsQueueIndex.value(),
                                                    m_queueIndices.presentQueueIndex.value(),
                                                    m_queueIndices.computeQueueIndex.value()};
    float                        queuePriority = 1.0f;

    for (auto index : uniqueQueueIndices) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(index)
            .setPQueuePriorities(&queuePriority)
            .setQueueCount(1);

        queueCreateInfos.push_back(queueCreateInfo);
    }

    createInfo.setQueueCreateInfos(queueCreateInfos).setPEnabledExtensionNames(extensions);

    m_device = m_physicalDevice.createDevice(createInfo);
}

void RenderBackend::QueryQueueFamilyIndices() {
    auto properties = m_physicalDevice.getQueueFamilyProperties();

    for (uint32_t i = 0; const auto& queueFamily : properties) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            WIND_CORE_INFO("Present queue index is {}", i);
            m_queueIndices.graphicsQueueIndex = i;
        }
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
            WIND_CORE_INFO("Compute queue index is {}", i);
            m_queueIndices.computeQueueIndex = i;
        }
        if (queueFamily.queueCount > 0 && m_physicalDevice.getSurfaceSupportKHR(i, m_surface)) {
            WIND_CORE_INFO("Present queue index is {}", i);
            m_queueIndices.presentQueueIndex = i;
        }
        if (m_queueIndices.IsComplete()) break;
        ++i;
    }
}

std::vector<const char*> RenderBackend::GetRequiredExtensions() {
    uint32_t     glfwEextensionsCnt = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwEextensionsCnt);
    std::vector<const char*> extensions(glfwEextensionsCnt);
    for (int i = 0; i < glfwEextensionsCnt; ++i) {
        extensions[i] = glfwExtensions[i];
        WIND_CORE_INFO(extensions[i]);
    }
    return extensions;
}

void RenderBackend::CreateSuface() {
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(m_vkInstance, m_createSetting.window.GetWindow(), nullptr,
                                &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
    m_surface = rawSurface;
}

void RenderBackend::GetQueue() {
    m_graphicsQueue = m_device.getQueue(m_queueIndices.graphicsQueueIndex.value(), 0);
    m_presentQueue  = m_device.getQueue(m_queueIndices.presentQueueIndex.value(), 0);
    m_computeQueue  = m_device.getQueue(m_queueIndices.computeQueueIndex.value(), 0);
}

void RenderBackend::CreateCmdPool() {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(m_queueIndices.graphicsQueueIndex.value());
    m_coomandPool = m_device.createCommandPool(createInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandPool(m_coomandPool);
}

void RenderBackend::QuerySurfaceProperty() {
    auto presentModes        = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
    auto surfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    auto surfaceFormats      = m_physicalDevice.getSurfaceFormatsKHR(m_surface);

    m_surfacePresentMode = vk::PresentModeKHR::eFifo;
    if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox) !=
        presentModes.end()) {
        m_surfacePresentMode = vk::PresentModeKHR::eMailbox;
        WIND_CORE_INFO("Using mailbox present mode");
    } else {
        WIND_CORE_INFO("Using fifo present mode");
    }

    m_surfaceFormat = surfaceFormats.front();
    for (const auto& availableFormat : surfaceFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            m_surfaceFormat = availableFormat;
            WIND_CORE_INFO("Find suit surface format");
            break;
        }
    }
}

void RenderBackend::RecreateSwapchain(uint32_t surfaceWidth, uint32_t surfaceHeight) {
    m_device.waitIdle(); // wait for sync
    auto surfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    m_surfaceExtent =
        vk::Extent2D(std::clamp(surfaceWidth, surfaceCapabilities.minImageExtent.width,
                                surfaceCapabilities.maxImageExtent.width),
                     std::clamp(surfaceHeight, surfaceCapabilities.minImageExtent.height,
                                surfaceCapabilities.maxImageExtent.height));
    m_renderingEnabled = true;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;

    swapchainCreateInfo.setSurface(m_surface)
        .setMinImageCount(m_createSetting.maxFrameInflight)
        .setImageFormat(m_surfaceFormat.format)
        .setImageColorSpace(m_surfaceFormat.colorSpace)
        .setImageExtent(m_surfaceExtent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                       vk::ImageUsageFlagBits::eTransferDst)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setPreTransform(surfaceCapabilities.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(m_surfacePresentMode)
        .setClipped(true)
        .setOldSwapchain(m_swapchain);

    m_swapchain = m_device.createSwapchainKHR(swapchainCreateInfo);

    if (static_cast<bool>(swapchainCreateInfo.oldSwapchain)) {
        m_device.destroySwapchainKHR(swapchainCreateInfo.oldSwapchain);
    }

    auto swapChainImage = m_device.getSwapchainImagesKHR(m_swapchain);
    m_presentImageCnt   = swapChainImage.size();
    m_swapchainImages.clear();
    m_swapchainImages.reserve(m_presentImageCnt);
    m_swapchainImageUsages.assign(m_presentImageCnt, ImageUsage::UNKNOWN);

    for (uint32_t i = 0; i < m_presentImageCnt; ++i) {
        m_swapchainImages.push_back(
            Image(swapChainImage[i], surfaceWidth, surfaceHeight, m_surfaceFormat.format));
    }
    WIND_CORE_INFO("Create swapchain successful");
}

void RenderBackend::CreateSyncObeject() {
    m_imageAvailableSemaphore    = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
    m_renderingFinishedSemaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
    m_immediateFence             = m_device.createFence(vk::FenceCreateInfo{});
    WIND_CORE_INFO("Create sync object");
}

void RenderBackend::CreateVmaAllocator() {
    VmaAllocatorCreateInfo allocatorInfo{};

    allocatorInfo.device           = m_device;
    allocatorInfo.physicalDevice   = m_physicalDevice;
    allocatorInfo.instance         = m_vkInstance;
    allocatorInfo.vulkanApiVersion = VK_MAKE_VERSION(1, 0, 3);

    vmaCreateAllocator(&allocatorInfo, &m_allocator);
    WIND_CORE_INFO("Create vulkan memory allocator");
}

void RenderBackend::CreateDescriptorCacheAndAllocator() {
    m_descriptorLayoutCache = std::make_shared<DescriptorLayoutCache>();
    m_descriptorLayoutCache->Init(m_device);

    m_descriptorAllocator = std::make_shared<DescriptorAllocator>();
    m_descriptorAllocator->Init(m_device);
}

CommandBuffer RenderBackend::BeginSingleTimeCommand() {
    vk::CommandBufferAllocateInfo allocateInfo;

    allocateInfo.setCommandBufferCount(1)
        .setCommandPool(m_coomandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    vk::CommandBuffer          immCmdBuffer = m_device.allocateCommandBuffers(allocateInfo).front();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    immCmdBuffer.begin(beginInfo);
    return CommandBuffer(immCmdBuffer);
}

void RenderBackend::SubmitSingleTimeCommand(vk::CommandBuffer cmdBuffer) {
    cmdBuffer.end();
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuffer);
    m_graphicsQueue.submit(submitInfo, {});
}

[[nodiscard]] std::vector<CommandBuffer>
RenderBackend::RequestMultiCommandBuffer(uint32_t count) {
    vk::CommandBufferAllocateInfo allocateInfo;

    allocateInfo.setCommandBufferCount(count)
        .setCommandPool(m_coomandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    std::vector<vk::CommandBuffer> allocCmdBuffers = m_device.allocateCommandBuffers(allocateInfo);
    std::vector<CommandBuffer> commandbuffers;
    for (auto& cmdBufferHandle : allocCmdBuffers) {
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBufferHandle.begin(beginInfo);
        commandbuffers.emplace_back(CommandBuffer{cmdBufferHandle});
    }
    return commandbuffers;
}

void RenderBackend::SubmitCommands(std::vector<CommandBuffer>& commandVecs) {
    for (auto& commands : commandVecs) {
        commands.End();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(commands.GetNativeHandle());
        m_graphicsQueue.submit(submitInfo, {});
    }
}
} // namespace wind