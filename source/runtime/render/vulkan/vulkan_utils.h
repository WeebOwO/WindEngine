#pragma once

#include <optional>
#include <cstdint>
#include <string_view>
#include <Vulkan/vulkan.h>
#include <vector>

namespace vk {
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool IsComplete() {
        return graphicsFamily.has_value();
    }
};
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
void CreateGraphicsPipeline(std::string_view vertFilepath, std::string_view fragFilepath);
bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers); 

}// namespace vk
