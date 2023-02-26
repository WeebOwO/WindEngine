#pragma once

#include <vulkan/vulkan.h>
#include "runtime/base/macro.h"

constexpr bool enableValidationLayers = true;

class VulkanRHI {
public:
    VulkanRHI();
    ~VulkanRHI();
private:
    void InitVulkan();
    void Cleanup();
    void CreateInstance();
    void PickPhysicalDevice();
    void CreateLogicDevice();
    void SetUpValidationLayers();
private:
    VkInstance m_vkInstance {VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice {VK_NULL_HANDLE};    
    VkDevice m_logicalDevice {VK_NULL_HANDLE};
};

