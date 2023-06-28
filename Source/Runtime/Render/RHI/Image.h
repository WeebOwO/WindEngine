#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Vma.h"

struct VkImage_T;
using VkImage = VkImage_T*;

struct VkImageView_T;
using VkImageView = VkImageView_T*;

namespace wind {
struct ImageUsage {
    using Value = uint32_t;
    enum Bits : Value {
        UNKNOWN                  = (Value)vk::ImageUsageFlagBits{},
        TRANSFER_SOURCE          = (Value)vk::ImageUsageFlagBits::eTransferSrc,
        TRANSFER_DESTINATION     = (Value)vk::ImageUsageFlagBits::eTransferDst,
        SHADER_READ              = (Value)vk::ImageUsageFlagBits::eSampled,
        STORAGE                  = (Value)vk::ImageUsageFlagBits::eStorage,
        COLOR_ATTACHMENT         = (Value)vk::ImageUsageFlagBits::eColorAttachment,
        DEPTH_SPENCIL_ATTACHMENT = (Value)vk::ImageUsageFlagBits::eDepthStencilAttachment,
        INPUT_ATTACHMENT         = (Value)vk::ImageUsageFlagBits::eInputAttachment,
        FRAGMENT_SHADING_RATE_ATTACHMENT =
            (Value)vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR,
    };
};

enum class ImageView {
    NATIVE = 0,
    DEPTH_ONLY,
    STENCIL_ONLY,
};

struct ImageOptions {
    using Value = uint32_t;

    enum Bits : Value {
        DEFAULT = 0,
        MIPMAPS = 1 << 0,
        CUBEMAP = 1 << 1,
    };
};

vk::ImageAspectFlags   ImageFormatToImageAspect(vk::Format format);
vk::ImageLayout        ImageUsageToImageLayout(ImageUsage::Bits usage);
vk::AccessFlags        ImageUsageToAccessFlags(ImageUsage::Bits usage);
vk::PipelineStageFlags ImageUsageToPipelineStage(ImageUsage::Bits usage);

class Image {
public:
    Image() = default;
    Image(uint32_t width, uint32_t height, vk::Format format, ImageUsage::Value usage,
          MemoryUsage memoryUsage, ImageOptions::Value options);
    Image(vk::Image image, uint32_t width, uint32_t height, vk::Format format);
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;
    ~Image();
    
    void Init(uint32_t width, uint32_t height, vk::Format format, ImageUsage::Value usage,
              MemoryUsage memoryUsage, ImageOptions::Value options);

    [[nodiscard]] vk::ImageView GetNativeView(ImageView view) const;
    [[nodiscard]] vk::ImageView GetNativeView(ImageView view, uint32_t layer) const;
    [[nodiscard]] uint32_t      GetMipLevelWidth(uint32_t mipLevel) const;
    [[nodiscard]] uint32_t      GetMipLevelHeight(uint32_t mipLevel) const;

    [[nodiscard]] auto GetNativeHandle() const { return m_handle; }
    [[nodiscard]] auto GetFormat() const { return m_format; }
    [[nodiscard]] auto GetWidth() const { return m_extent.width; }
    [[nodiscard]] auto GetHeight() const { return m_extent.height; }
    [[nodiscard]] auto GetMipLevelCount() const { return m_mipLevelCount; }
    [[nodiscard]] auto GetLayerCount() const { return m_layerCount; }

private:
    void Destroy();
    void InitViews(const vk::Image& image, vk::Format format);

    struct ImageViews {
        vk::ImageView nativeView;
        vk::ImageView depthOnlyView;
        vk::ImageView stencilOnlyView;
    };

    vk::Image               m_handle;
    ImageViews              m_defaultImageViews;
    std::vector<ImageViews> m_cubemapImageViews;

    vk::Extent2D  m_extent{0u, 0u};
    uint32_t      m_mipLevelCount{1};
    uint32_t      m_layerCount{1};
    vk::Format    m_format{vk::Format::eUndefined};
    VmaAllocation m_allocation = {};
};

vk::ImageSubresourceLayers GetDefaultImageSubresourceLayers(const Image& image);
vk::ImageSubresourceLayers GetDefaultImageSubresourceLayers(const Image& image, uint32_t mipLevel,
                                                            uint32_t layer);
vk::ImageSubresourceRange  GetDefaultImageSubresourceRange(const Image& image);

uint32_t CalculateImageMipLevelCount(ImageOptions::Value options, uint32_t width, uint32_t height);
uint32_t CalculateImageLayerCount(ImageOptions::Value options);

using ImageReference = std::reference_wrapper<const Image>;
} // namespace wind