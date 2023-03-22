#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace wind {
    struct SwapChain {
        vk::SwapchainKHR swapChain;
        
        SwapChain(uint32_t width, uint32_t height);
    };
}