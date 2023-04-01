#include "runtime/render/context.h"

#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "GLFW/glfw3.h"
#include "runtime/base/macro.h"
#include "runtime/base/misc.h"

static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

namespace wind {
std::unique_ptr<RenderContext> RenderContext::instance = nullptr;

RenderContext::RenderContext(GLFWwindow* window) noexcept {
    CreateInstance();
    PickupPhysicalDevice();
    CreateSuface(window);
    QueryQueueFamilyIndices();
    CreateDevice();
    GetQueue();
    CreateCmdPool();
}

void RenderContext::CreateInstance() {
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

    vkInstance = vk::createInstance(createinfo);
}

RenderContext::~RenderContext() {
    device.destroyCommandPool(graphicsCmdPool);
    device.destroy();
    vkInstance.destroySurfaceKHR(surface);
    vkInstance.destroy();
}

void RenderContext::PickupPhysicalDevice() {
    auto physicalDevices = vkInstance.enumeratePhysicalDevices();
    // not good, but enough for me
    physicalDevice = physicalDevices.front();
    WIND_CORE_INFO(physicalDevice.getProperties().deviceName);
}

void RenderContext::CreateDevice() {
    std::array           extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    vk::DeviceCreateInfo createInfo;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::unordered_set<uint32_t> uniqueQueueIndices{queueIndices.graphicsQueueIndex.value(),
                                                    queueIndices.presentQueueIndex.value()};
    float                        queuePriority = 1.0f;

    for (auto index : uniqueQueueIndices) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(index)
            .setPQueuePriorities(&queuePriority)
            .setQueueCount(1);

        queueCreateInfos.push_back(queueCreateInfo);
    }

    createInfo.setQueueCreateInfos(queueCreateInfos).setPEnabledExtensionNames(extensions);

    device = physicalDevice.createDevice(createInfo);
}

void RenderContext::QueryQueueFamilyIndices() {
    auto properties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; const auto& queueFamily : properties) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            queueIndices.graphicsQueueIndex = i;
        }
        if (queueFamily.queueCount > 0 && physicalDevice.getSurfaceSupportKHR(i, surface)) {
            queueIndices.presentQueueIndex = i;
        }
        if (queueIndices.IsComplete()) break;
        ++i;
    }
}

std::vector<const char*> RenderContext::GetRequiredExtensions() {
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

void RenderContext::CreateSuface(GLFWwindow* window) {
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(vkInstance, window, nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    surface = rawSurface;
}

void RenderContext::GetQueue() {
    graphicsQueue = device.getQueue(queueIndices.graphicsQueueIndex.value(), 0);
    presentQueue  = device.getQueue(queueIndices.presentQueueIndex.value(), 0);
}

void RenderContext::CreateCmdPool() {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(queueIndices.graphicsQueueIndex.value());
    graphicsCmdPool = device.createCommandPool(createInfo);
}

} // namespace wind