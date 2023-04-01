#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"
#include "runtime/base/macro.h"
#include "runtime/render/shader.h"

namespace wind {

class RenderContext final {
public:
    PERMIT_COPY(RenderContext);
    PERMIT_MOVE(RenderContext);

    RenderContext(GLFWwindow* window) noexcept;

    struct QueueIndices final {
        std::optional<uint32_t> graphicsQueueIndex;
        std::optional<uint32_t> presentQueueIndex;
        bool IsComplete() { return graphicsQueueIndex.has_value() && presentQueueIndex.has_value(); }
    };

    ~RenderContext();
    
    static void Init(GLFWwindow* window) {instance = std::make_unique<RenderContext>(window);}
    
    static RenderContext& GetInstace() {
        assert(instance);
        return *instance;
    }
    
    vk::Instance       vkInstance;
    vk::PhysicalDevice physicalDevice;
    vk::Device         device;
    vk::SurfaceKHR     surface;

    vk::Queue          graphicsQueue;
    vk::Queue          presentQueue;
    
    std::unordered_map<size_t, Shader> shaderCache;

    vk::CommandPool graphicsCmdPool;

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
    void                     CreateCmdPool();
    static std::unique_ptr<RenderContext> instance;
};


}; // namespace wind