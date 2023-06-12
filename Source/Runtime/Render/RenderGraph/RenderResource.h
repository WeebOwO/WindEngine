#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Vma.h"

namespace wind {
struct TextureDesc {
    uint32_t            width;
    uint32_t            height;
    vk::Format          format;
    ImageUsage::Value   usage;
    MemoryUsage         memoryUsage;
    ImageOptions::Value options;
};

struct RDGRenderTarget {
    uint32_t width;
    uint32_t height;
    Image    colorAttachment;
    Image    depthAttachment;
};

struct BufferDesc {};
} // namespace wind