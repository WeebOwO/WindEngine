#include "vulkan_rhi.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <GLFW/glfw3.h>

#include "runtime/render/vulkan/vulkan_rhi.h"
#include "runtime/render/vulkan/vulkan_utils.h"
#include "vulkan_utils.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT             messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VulkanRHI::VulkanRHI() { InitVulkan(); }

VulkanRHI::~VulkanRHI() { vkDestroyInstance(m_vkInstance, nullptr); }

void VulkanRHI::InitVulkan() {
    CreateInstance();
    PickPhysicalDevice();
}

void VulkanRHI::CreateInstance() {
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    if (enableValidationLayers && !vk::CheckValidationLayerSupport(validationLayers)) {
        throw std::runtime_error("ValidationLayers not support!\n");
    }

    VkApplicationInfo vkAppInfo{};
    vkAppInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName   = "Hello Triangle";
    vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.pEngineName        = "No Engine";
    vkAppInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo vkCreateInfo{};
    vkCreateInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkCreateInfo.pApplicationInfo = &vkAppInfo;

    // get extensions
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    vkCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    vkCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enableValidationLayers) {
        vkCreateInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        vkCreateInfo.ppEnabledLayerNames = validationLayers.data();

        debugCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;

        vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        vkCreateInfo.enabledLayerCount = 0;
        vkCreateInfo.pNext             = nullptr;
    }

    VkResult createResult = vkCreateInstance(&vkCreateInfo, nullptr, &m_vkInstance);
    assert(createResult == VK_SUCCESS);
}

void VulkanRHI::PickPhysicalDevice() {

    uint32_t deviceCnt = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCnt, nullptr);

    if (deviceCnt == 0) { throw std::runtime_error("No graphics card!\n"); }

    std::vector<VkPhysicalDevice> devices(deviceCnt);

    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCnt, devices.data());
    auto isDeviceSuitable = [](VkPhysicalDevice device) {
        vk::QueueFamilyIndices indices = vk::FindQueueFamilies(device);
        return indices.IsComplete();
    };

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable graphics card!");
    }
}

void VulkanRHI::CreateLogicDevice() {
    vk::QueueFamilyIndices indices = vk::FindQueueFamilies(m_physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
}
