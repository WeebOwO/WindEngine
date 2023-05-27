#include "Buffer.h"

#include "Runtime/Render/Rhi/Backend.h"

namespace wind {

Buffer::Buffer(size_t byteSize, BufferUsage::Value usage, MemoryUsage memoryUsage) {
    Init(byteSize, usage, memoryUsage);
}

void Buffer::Init(size_t byteSize, BufferUsage::Value usage, MemoryUsage memoryUsage) {
    constexpr std::array BufferQueueFamiliyIndicies = {(uint32_t)0};
    // destroy previous buffer
    Destroy();

    m_byteSize = byteSize;
    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.setSize(m_byteSize)
        .setUsage((vk::BufferUsageFlags)usage)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setQueueFamilyIndices(BufferQueueFamiliyIndicies);

    m_allocation = AllocateBuffer(bufferCreateInfo, memoryUsage, &m_handle);
}

bool Buffer::IsMemoryMapped() const { return m_mappedMemory != nullptr; }

uint8_t* Buffer::MapMemory() {
    if (m_mappedMemory == nullptr) m_mappedMemory = wind::MapMemory(m_allocation);
    return m_mappedMemory;
}

void Buffer::UnmapMemory() {
    wind::UnmapMemory(m_allocation);
    m_mappedMemory = nullptr;
}

void Buffer::FlushMemory() { FlushMemory(m_byteSize, 0); }

void Buffer::FlushMemory(size_t byteSize, size_t offset) {
    wind::FlushMemory(m_allocation, byteSize, offset);
}

void Buffer::CopyData(const uint8_t* data, size_t byteSize, size_t offset) {
    assert(byteSize + offset <= byteSize);

    if (m_mappedMemory == nullptr) {
        (void)MapMemory();
        std::memcpy((void*)(m_mappedMemory + offset), (const void*)data, byteSize);
        FlushMemory(byteSize, offset);
        UnmapMemory();
    } else {
        std::memcpy((void*)(m_mappedMemory + offset), (const void*)data, byteSize);
    }
}

void Buffer::CopyDataWithFlush(const uint8_t* data, size_t byteSize, size_t offset) {
    CopyData(data, byteSize, offset);
    if (IsMemoryMapped()) FlushMemory(byteSize, offset);
}

void Buffer::Destroy() {
    if ((bool)m_handle) {
        if (m_mappedMemory != nullptr) UnmapMemory();
        DeallocateBuffer(m_handle, m_allocation);
    }
}

Buffer::Buffer(Buffer&& other) noexcept {
    m_handle       = other.m_handle;
    m_byteSize     = other.m_byteSize;
    m_allocation   = other.m_allocation;
    m_mappedMemory = other.m_mappedMemory;

    other.m_handle       = vk::Buffer{};
    other.m_byteSize     = 0;
    other.m_allocation   = {};
    other.m_mappedMemory = nullptr;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    Destroy();

    m_handle       = other.m_handle;
    m_byteSize     = other.m_byteSize;
    m_allocation   = other.m_allocation;
    m_mappedMemory = other.m_mappedMemory;

    other.m_handle       = vk::Buffer{};
    other.m_byteSize     = 0;
    other.m_allocation   = {};
    other.m_mappedMemory = nullptr;

    return *this;
}

Buffer::~Buffer() { Destroy(); }
} // namespace wind