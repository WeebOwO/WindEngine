#include "vulkan_utils.h"

#include <format>
#include <stdexcept>
#include <cstdint>
#include <cstring>

#include "runtime/base/io.h"
#include "runtime/render/vulkan/vulkan_utils.h"

namespace vk {

void CreateGraphicsPipeline(std::string_view vertFilepath, std::string_view fragFilepath) {
    auto vertexCode = io::readFile(vertFilepath);
    auto fragCode = io::readFile(fragFilepath);
    std::cout << std::format("Vertexshader code size is {}\n", vertexCode.size());
    std::cout << std::format("Fragshader code size is {}\n", fragCode.size());
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    uint32_t pQueueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &pQueueFamilyPropertyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(pQueueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &pQueueFamilyPropertyCount, queueFamilies.data());
    
    for(uint32_t i = 0; const auto& queueFamilyProperty: queueFamilies) {
        if(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i; 
        }
        if(indices.IsComplete()) {
            break;
        }
        ++i;
    }
    return indices;
}

bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers) {
        bool layerFound = false;
        for(const auto& layerProps : availableLayers) {
            if(strcmp(layerName, layerProps.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if(!layerFound) {
            return false;
        }
    }

    return true;
}
} // namespace vk