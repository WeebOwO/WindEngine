#pragma once

#include "vulkan/vulkan.hpp"

namespace wind {
class Buffer {
public:
    vk::Buffer       buffer;
    vk::DeviceMemory memory;
    size_t           size;

    Buffer(vk::DeviceSize size, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags property);
    ~Buffer();
private:
    void QueryMemoryInfo();
    
    struct MemoryInfo {
        size_t size;
        uint32_t index;
    };
    
    MemoryInfo m_info;
};
} // namespace wind
