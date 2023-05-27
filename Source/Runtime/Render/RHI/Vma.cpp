#include "Vma.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "Runtime/Render/RHI/Backend.h"

namespace wind {
VmaMemoryUsage MemoryUsageToNative(MemoryUsage usage) {
    constexpr VmaMemoryUsage mappingTable[] = {
        VMA_MEMORY_USAGE_GPU_ONLY,   VMA_MEMORY_USAGE_CPU_ONLY,
        VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_MEMORY_USAGE_GPU_TO_CPU,
        VMA_MEMORY_USAGE_CPU_COPY,   VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED,
    };
    return mappingTable[(size_t)usage];
}

VmaAllocator GetVulkanAllocator() { return RenderBackend::GetInstance().GetAllocator(); }

void DeallocateImage(const vk::Image& image, VmaAllocation allocation) {
    vmaDestroyImage(GetVulkanAllocator(), image, allocation);
}

void DeallocateBuffer(const vk::Buffer& buffer, VmaAllocation allocation) {
    vmaDestroyBuffer(GetVulkanAllocator(), buffer, allocation);
}

VmaAllocation AllocateImage(const vk::ImageCreateInfo& imageCreateInfo, MemoryUsage usage,
                            vk::Image* image) {
    VmaAllocation           allocation     = {};
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage                   = MemoryUsageToNative(usage);
    (void)vmaCreateImage(RenderBackend::GetInstance().GetAllocator(),
                         (VkImageCreateInfo*)&imageCreateInfo, &allocationInfo, (VkImage*)image,
                         &allocation, nullptr);
    return allocation;
}

VmaAllocation AllocateBuffer(const vk::BufferCreateInfo& bufferCreateInfo, MemoryUsage usage,
                             vk::Buffer* buffer) {
    VmaAllocation           allocation     = {};
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage                   = MemoryUsageToNative(usage);
    (void)vmaCreateBuffer(RenderBackend::GetInstance().GetAllocator(),
                          (VkBufferCreateInfo*)&bufferCreateInfo, &allocationInfo,
                          (VkBuffer*)buffer, &allocation, nullptr);
    return allocation;
}

uint8_t* MapMemory(VmaAllocation allocation) {
    void* memory = nullptr;
    vmaMapMemory(RenderBackend::GetInstance().GetAllocator(), allocation, &memory);
    return (uint8_t*)memory;
}

void UnmapMemory(VmaAllocation allocation) {
    vmaUnmapMemory(RenderBackend::GetInstance().GetAllocator(), allocation);
}

void FlushMemory(VmaAllocation allocation, size_t byteSize, size_t offset) {
    vmaFlushAllocation(RenderBackend::GetInstance().GetAllocator(), allocation, offset, byteSize);
}
} // namespace wind
