#include "runtime/render/buffer.h"

#include "runtime/render/context.h"
#include "runtime/render/utils.h"

namespace wind {
    Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags property): size(size) {
        auto& device = RenderContext::GetInstace().device;
        auto& phyDevice = RenderContext::GetInstace().physicalDevice;
        // create buffer
        vk::BufferCreateInfo createInfo;
        createInfo.setSize(size)
                  .setUsage(usage)
                  .setSharingMode(vk::SharingMode::eExclusive);

        buffer = device.createBuffer(createInfo);
        // query memory info
        auto properties = phyDevice.getMemoryProperties();
        auto requirements = device.getBufferMemoryRequirements(buffer);

        m_info.size = requirements.size;

        for(int i = 0; i < properties.memoryTypeCount; ++i) {
            if(((1 << i) & requirements.memoryTypeBits) && ((properties.memoryTypes[i].propertyFlags & property) == property)) {
                m_info.index = i;
                break;
            }
        }

        vk::MemoryAllocateInfo allocateInfo;
        allocateInfo.setMemoryTypeIndex(m_info.index)
                    .setAllocationSize(m_info.size);
        
        memory = device.allocateMemory(allocateInfo);

        device.bindBufferMemory(buffer, memory, 0);
    }
    
    Buffer::~Buffer() {
        auto& device = RenderContext::GetInstace().device;
        device.freeMemory(memory);
        device.destroyBuffer(buffer);
    }
}