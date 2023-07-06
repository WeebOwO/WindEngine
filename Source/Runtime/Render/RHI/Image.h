#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Vma.h"

struct VkImage_T;
using VkImage = VkImage_T*;

struct VkImageView_T;
using VkImageView = VkImageView_T*;

namespace wind {

enum class Format : uint32_t {
    UNDEFINED = 0,
    R4G4_UNORM_PACK_8,
    R4G4B4A4_UNORM_PACK_16,
    B4G4R4A4_UNORM_PACK_16,
    R5G6B5_UNORM_PACK_16,
    B5G6R5_UNORM_PACK_16,
    R5G5B5A1_UNORM_PACK_16,
    B5G5R5A1_UNORM_PACK_16,
    A1R5G5B5_UNORM_PACK_16,
    R8_UNORM,
    R8_SNORM,
    R8_USCALED,
    R8_SSCALED,
    R8_UINT,
    R8_SINT,
    R8_SRGB,
    R8G8_UNORM,
    R8G8_SNORM,
    R8G8_USCALED,
    R8G8_SSCALED,
    R8G8_UINT,
    R8G8_SINT,
    R8G8_SRGB,
    R8G8B8_UNORM,
    R8G8B8_SNORM,
    R8G8B8_USCALED,
    R8G8B8_SSCALED,
    R8G8B8_UINT,
    R8G8B8_SINT,
    R8G8B8_SRGB,
    B8G8R8_UNORM,
    B8G8R8_SNORM,
    B8G8R8_USCALED,
    B8G8R8_SSCALED,
    B8G8R8_UINT,
    B8G8R8_SINT,
    B8G8R8_SRGB,
    R8G8B8A8_UNORM,
    R8G8B8A8_SNORM,
    R8G8B8A8_USCALED,
    R8G8B8A8_SSCALED,
    R8G8B8A8_UINT,
    R8G8B8A8_SINT,
    R8G8B8A8_SRGB,
    B8G8R8A8_UNORM,
    B8G8R8A8_SNORM,
    B8G8R8A8_USCALED,
    B8G8R8A8_SSCALED,
    B8G8R8A8_UINT,
    B8G8R8A8_SINT,
    B8G8R8A8_SRGB,
    A8B8G8R8_UNORM_PACK_32,
    A8B8G8R8_SNORM_PACK_32,
    A8B8G8R8_USCALED_PACK_32,
    A8B8G8R8_SSCALED_PACK_32,
    A8B8G8R8_UINT_PACK_32,
    A8B8G8R8_SINT_PACK_32,
    A8B8G8R8_SRGB_PACK_32,
    A2R10G10B10_UNORM_PACK_32,
    A2R10G10B10_SNORM_PACK_32,
    A2R10G10B10_USCALED_PACK_32,
    A2R10G10B10_SSCALED_PACK_32,
    A2R10G10B10_UINT_PACK_32,
    A2R10G10B10_SINT_PACK_32,
    A2B10G10R10_UNORM_PACK_32,
    A2B10G10R10_SNORM_PACK_32,
    A2B10G10R10_USCALED_PACK_32,
    A2B10G10R10_SSCALED_PACK_32,
    A2B10G10R10_UINT_PACK_32,
    A2B10G10R10_SINT_PACK_32,
    R16_UNORM,
    R16_SNORM,
    R16_USCALED,
    R16_SSCALED,
    R16_UINT,
    R16_SINT,
    R16_SFLOAT,
    R16G16_UNORM,
    R16G16_SNORM,
    R16G16_USCALED,
    R16G16_SSCALED,
    R16G16_UINT,
    R16G16_SINT,
    R16G16_SFLOAT,
    R16G16B16_UNORM,
    R16G16B16_SNORM,
    R16G16B16_USCALED,
    R16G16B16_SSCALED,
    R16G16B16_UINT,
    R16G16B16_SINT,
    R16G16B16_SFLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_SNORM,
    R16G16B16A16_USCALED,
    R16G16B16A16_SSCALED,
    R16G16B16A16_UINT,
    R16G16B16A16_SINT,
    R16G16B16A16_SFLOAT,
    R32_UINT,
    R32_SINT,
    R32_SFLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R32G32_SFLOAT,
    R32G32B32_UINT,
    R32G32B32_SINT,
    R32G32B32_SFLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    R32G32B32A32_SFLOAT,
    R64_UINT,
    R64_SINT,
    R64_SFLOAT,
    R64G64_UINT,
    R64G64_SINT,
    R64G64_SFLOAT,
    R64G64B64_UINT,
    R64G64B64_SINT,
    R64G64B64_SFLOAT,
    R64G64B64A64_UINT,
    R64G64B64A64_SINT,
    R64G64B64A64_SFLOAT,
    B10G11R11_UFLOAT_PACK_32,
    E5B9G9R9_UFLOAT_PACK_32,
    D16_UNORM,
    X8D24_UNORM_PACK_32,
    D32_SFLOAT,
    S8_UINT,
    D16_UNORM_S8_UINT,
    D24_UNORM_S8_UINT,
    D32_SFLOAT_S8_UINT,
};

const vk::Format& ToNative(Format format);
Format FromNative(const vk::Format& format);

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