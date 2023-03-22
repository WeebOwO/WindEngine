#pragma once

#include <iostream>
#include <memory>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"
#include "runtime/base/macro.h"

namespace wind {

class RenderContext final {
public:
    PERMIT_COPY(RenderContext);
    PERMIT_MOVE(RenderContext);

    RenderContext(GLFWwindow* window) noexcept;
    struct QueueIndices final {
        std::optional<uint32_t> graphicsQueueIndex;
        std::optional<uint32_t> presentQueueIndex;
        bool IsComplete() { return graphicsQueueIndex.value() && presentQueueIndex.value(); }
    };

    ~RenderContext();

    vk::Instance       vkInstance;
    vk::PhysicalDevice physicalDevice;
    vk::Device         device;
    vk::SurfaceKHR     surface;

    vk::Queue          graphicsQueue;
    vk::Queue          presentQueue;

    QueueIndices queueIndices;

private:
    std::vector<const char*> GetRequiredExtensions();
    void                     CreateInstance();
    void                     PickupPhysicalDevice();
    void                     CreateDevice();
    void                     CreateSuface(GLFWwindow* window);
    void                     QueryQueueFamilyIndices();
    void                     CreateSwapChain();
    void                     GetQueue();
};

}; // namespace wind