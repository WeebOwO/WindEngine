#pragma once

#include <iostream>
#include <memory>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "macro.h"

namespace wind {
    
class RenderContext final {
public:
    PERMIT_COPY(RenderContext);
    struct QueueIndices final {
        std::optional<uint32_t> graphicsQueueIndex;
        bool IsComplete() {
            return graphicsQueueIndex.value();
        }   
    };

    static void Init();
    static void Quit();

    [[nodiscard]] static RenderContext& GetInstance() {
        assert(m_instance);
        return *m_instance;
    }

    ~RenderContext();

    vk::Instance vkInstance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    QueueIndices queueIndices;    
    
private:
    RenderContext() noexcept;    
    void CreateInstance();
    void PickupPhysicalDevice();
    void CreateDevice();
    void QueryQueueFamilyIndices();
    void GetQueue();
    static std::unique_ptr<RenderContext> m_instance;
};  
}; // namespace wind