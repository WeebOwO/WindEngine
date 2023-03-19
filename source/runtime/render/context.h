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

    RenderContext(GLFWwindow* window) noexcept {
        CreateInstance();
        PickupPhysicalDevice();
        QueryQueueFamilyIndices();
        CreateDevice();
        GetQueue();
    }
    struct QueueIndices final {
        std::optional<uint32_t> graphicsQueueIndex;
        bool                    IsComplete() { return graphicsQueueIndex.value(); }
    };

    ~RenderContext();

    vk::Instance       vkInstance;
    vk::PhysicalDevice physicalDevice;
    vk::Device         device;
    vk::Queue          graphicsQueue;
    vk::SurfaceKHR     surface;

    QueueIndices queueIndices;

private:
    void CreateInstance();
    void PickupPhysicalDevice();
    void CreateDevice();
    void QueryQueueFamilyIndices();
    void CreateSwapChain();
    void GetQueue();
};

}; // namespace wind