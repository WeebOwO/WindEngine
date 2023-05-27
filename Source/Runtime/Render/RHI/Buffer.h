#pragma once

#include "RUntime/Render/RHI/Vma.h"

#include <vulkan/vulkan.hpp>

struct VkBuffer_T;
using VkBuffer = VkBuffer_T*;

namespace wind {
struct BufferUsage {
    using Value = uint32_t;

    enum Bits : Value {
        UNKNOWN                   = (Value)vk::BufferUsageFlags{},
        TRANSFER_SOURCE           = (Value)vk::BufferUsageFlagBits::eTransferSrc,
        TRANSFER_DESTINATION      = (Value)vk::BufferUsageFlagBits::eTransferDst,
        UNIFORM_TEXEL_BUFFER      = (Value)vk::BufferUsageFlagBits::eUniformTexelBuffer,
        STORAGE_TEXEL_BUFFER      = (Value)vk::BufferUsageFlagBits::eStorageTexelBuffer,
        UNIFORM_BUFFER            = (Value)vk::BufferUsageFlagBits::eUniformBuffer,
        STORAGE_BUFFER            = (Value)vk::BufferUsageFlagBits::eStorageBuffer,
        INDEX_BUFFER              = (Value)vk::BufferUsageFlagBits::eIndexBuffer,
        VERTEX_BUFFER             = (Value)vk::BufferUsageFlagBits::eVertexBuffer,
        INDIRECT_BUFFER           = (Value)vk::BufferUsageFlagBits::eIndirectBuffer,
        SHADER_DEVICE_ADDRESS     = (Value)vk::BufferUsageFlagBits::eShaderDeviceAddress,
        TRANSFORM_FEEDBACK_BUFFER = (Value)vk::BufferUsageFlagBits::eTransformFeedbackBufferEXT,
        TRANSFORM_FEEDBACK_COUNTER_BUFFER =
            (Value)vk::BufferUsageFlagBits::eTransformFeedbackCounterBufferEXT,
        CONDITIONAL_RENDERING = (Value)vk::BufferUsageFlagBits::eConditionalRenderingEXT,
        ACCELERATION_STRUCTURE_BUILD_INPUT_READONLY =
            (Value)vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR,
        ACCELERATION_STRUCTURE_STORAGE =
            (Value)vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR,
        SHADER_BINDING_TABLE = (Value)vk::BufferUsageFlagBits::eShaderBindingTableKHR,
    };
};

class Buffer {
public:
    Buffer()                         = default;
    Buffer(const Buffer&)            = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    ~Buffer();

    Buffer(size_t byteSize, BufferUsage::Value usage, MemoryUsage memoryUsage);
    void Init(size_t byteSize, BufferUsage::Value usage, MemoryUsage memoryUsage);

    [[nodiscard]] auto GetNativeHandle() const { return m_handle; }
    [[nodiscard]] auto GetByteSize() const { return m_byteSize; }
    [[nodiscard]] bool IsMemoryMapped() const;

    uint8_t* MapMemory();
    void     UnmapMemory();
    void     FlushMemory();
    void     FlushMemory(size_t byteSize, size_t offset);
    void     CopyData(const uint8_t* data, size_t byteSize, size_t offset);
    void     CopyDataWithFlush(const uint8_t* data, size_t byteSize, size_t offset);

private:
    vk::Buffer    m_handle{nullptr};
    size_t        m_byteSize{0};
    VmaAllocation m_allocation{};
    uint8_t*      m_mappedMemory{nullptr};

    void Destroy();
};

using BufferReference = std::reference_wrapper<const Buffer>;
} // namespace wind
