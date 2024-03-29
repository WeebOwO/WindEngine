#include "Image.h"

#include "Runtime/Render/RHI/Backend.h"

namespace wind {
static vk::Format FormatTable[] = {
    vk::Format::eUndefined,
    vk::Format::eR4G4UnormPack8,
    vk::Format::eR4G4B4A4UnormPack16,
    vk::Format::eB4G4R4A4UnormPack16,
    vk::Format::eR5G6B5UnormPack16,
    vk::Format::eB5G6R5UnormPack16,
    vk::Format::eR5G5B5A1UnormPack16,
    vk::Format::eB5G5R5A1UnormPack16,
    vk::Format::eA1R5G5B5UnormPack16,
    vk::Format::eR8Unorm,
    vk::Format::eR8Snorm,
    vk::Format::eR8Uscaled,
    vk::Format::eR8Sscaled,
    vk::Format::eR8Uint,
    vk::Format::eR8Sint,
    vk::Format::eR8Srgb,
    vk::Format::eR8G8Unorm,
    vk::Format::eR8G8Snorm,
    vk::Format::eR8G8Uscaled,
    vk::Format::eR8G8Sscaled,
    vk::Format::eR8G8Uint,
    vk::Format::eR8G8Sint,
    vk::Format::eR8G8Srgb,
    vk::Format::eR8G8B8Unorm,
    vk::Format::eR8G8B8Snorm,
    vk::Format::eR8G8B8Uscaled,
    vk::Format::eR8G8B8Sscaled,
    vk::Format::eR8G8B8Uint,
    vk::Format::eR8G8B8Sint,
    vk::Format::eR8G8B8Srgb,
    vk::Format::eB8G8R8Unorm,
    vk::Format::eB8G8R8Snorm,
    vk::Format::eB8G8R8Uscaled,
    vk::Format::eB8G8R8Sscaled,
    vk::Format::eB8G8R8Uint,
    vk::Format::eB8G8R8Sint,
    vk::Format::eB8G8R8Srgb,
    vk::Format::eR8G8B8A8Unorm,
    vk::Format::eR8G8B8A8Snorm,
    vk::Format::eR8G8B8A8Uscaled,
    vk::Format::eR8G8B8A8Sscaled,
    vk::Format::eR8G8B8A8Uint,
    vk::Format::eR8G8B8A8Sint,
    vk::Format::eR8G8B8A8Srgb,
    vk::Format::eB8G8R8A8Unorm,
    vk::Format::eB8G8R8A8Snorm,
    vk::Format::eB8G8R8A8Uscaled,
    vk::Format::eB8G8R8A8Sscaled,
    vk::Format::eB8G8R8A8Uint,
    vk::Format::eB8G8R8A8Sint,
    vk::Format::eB8G8R8A8Srgb,
    vk::Format::eA8B8G8R8UnormPack32,
    vk::Format::eA8B8G8R8SnormPack32,
    vk::Format::eA8B8G8R8UscaledPack32,
    vk::Format::eA8B8G8R8SscaledPack32,
    vk::Format::eA8B8G8R8UintPack32,
    vk::Format::eA8B8G8R8SintPack32,
    vk::Format::eA8B8G8R8SrgbPack32,
    vk::Format::eA2R10G10B10UnormPack32,
    vk::Format::eA2R10G10B10SnormPack32,
    vk::Format::eA2R10G10B10UscaledPack32,
    vk::Format::eA2R10G10B10SscaledPack32,
    vk::Format::eA2R10G10B10UintPack32,
    vk::Format::eA2R10G10B10SintPack32,
    vk::Format::eA2B10G10R10UnormPack32,
    vk::Format::eA2B10G10R10SnormPack32,
    vk::Format::eA2B10G10R10UscaledPack32,
    vk::Format::eA2B10G10R10SscaledPack32,
    vk::Format::eA2B10G10R10UintPack32,
    vk::Format::eA2B10G10R10SintPack32,
    vk::Format::eR16Unorm,
    vk::Format::eR16Snorm,
    vk::Format::eR16Uscaled,
    vk::Format::eR16Sscaled,
    vk::Format::eR16Uint,
    vk::Format::eR16Sint,
    vk::Format::eR16Sfloat,
    vk::Format::eR16G16Unorm,
    vk::Format::eR16G16Snorm,
    vk::Format::eR16G16Uscaled,
    vk::Format::eR16G16Sscaled,
    vk::Format::eR16G16Uint,
    vk::Format::eR16G16Sint,
    vk::Format::eR16G16Sfloat,
    vk::Format::eR16G16B16Unorm,
    vk::Format::eR16G16B16Snorm,
    vk::Format::eR16G16B16Uscaled,
    vk::Format::eR16G16B16Sscaled,
    vk::Format::eR16G16B16Uint,
    vk::Format::eR16G16B16Sint,
    vk::Format::eR16G16B16Sfloat,
    vk::Format::eR16G16B16A16Unorm,
    vk::Format::eR16G16B16A16Snorm,
    vk::Format::eR16G16B16A16Uscaled,
    vk::Format::eR16G16B16A16Sscaled,
    vk::Format::eR16G16B16A16Uint,
    vk::Format::eR16G16B16A16Sint,
    vk::Format::eR16G16B16A16Sfloat,
    vk::Format::eR32Uint,
    vk::Format::eR32Sint,
    vk::Format::eR32Sfloat,
    vk::Format::eR32G32Uint,
    vk::Format::eR32G32Sint,
    vk::Format::eR32G32Sfloat,
    vk::Format::eR32G32B32Uint,
    vk::Format::eR32G32B32Sint,
    vk::Format::eR32G32B32Sfloat,
    vk::Format::eR32G32B32A32Uint,
    vk::Format::eR32G32B32A32Sint,
    vk::Format::eR32G32B32A32Sfloat,
    vk::Format::eR64Uint,
    vk::Format::eR64Sint,
    vk::Format::eR64Sfloat,
    vk::Format::eR64G64Uint,
    vk::Format::eR64G64Sint,
    vk::Format::eR64G64Sfloat,
    vk::Format::eR64G64B64Uint,
    vk::Format::eR64G64B64Sint,
    vk::Format::eR64G64B64Sfloat,
    vk::Format::eR64G64B64A64Uint,
    vk::Format::eR64G64B64A64Sint,
    vk::Format::eR64G64B64A64Sfloat,
    vk::Format::eB10G11R11UfloatPack32,
    vk::Format::eE5B9G9R9UfloatPack32,
    vk::Format::eD16Unorm,
    vk::Format::eX8D24UnormPack32,
    vk::Format::eD32Sfloat,
    vk::Format::eS8Uint,
    vk::Format::eD16UnormS8Uint,
    vk::Format::eD24UnormS8Uint,
    vk::Format::eD32SfloatS8Uint,
};

const vk::Format& ToNative(Format format) { return FormatTable[(size_t)format]; }

Format FromNative(const vk::Format& format) {
    for (size_t i = 0; i < std::size(FormatTable); i++) {
        if (FormatTable[i] == format) return (Format)i;
    }
    assert(false);
    return (Format)0;
}

vk::ImageViewType GetImageViewType(const Image& image) {
    return image.GetLayerCount() == 1 ? vk::ImageViewType::e2D : vk::ImageViewType::eCube;
}

vk::ImageAspectFlags ImageFormatToImageAspect(vk::Format format) {
    switch (format) {
    case vk::Format::eD16Unorm:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eX8D24UnormPack32:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eD32Sfloat:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eD16UnormS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD24UnormS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    default:
        return vk::ImageAspectFlagBits::eColor;
    }
}

vk::ImageLayout ImageUsageToImageLayout(ImageUsage::Bits layout) {
    switch (layout) {
    case wind::ImageUsage::UNKNOWN:
        return vk::ImageLayout::eUndefined;
    case wind::ImageUsage::TRANSFER_SOURCE:
        return vk::ImageLayout::eTransferSrcOptimal;
    case wind::ImageUsage::TRANSFER_DESTINATION:
        return vk::ImageLayout::eTransferDstOptimal;
    case wind::ImageUsage::SHADER_READ:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    case wind::ImageUsage::STORAGE:
        return vk::ImageLayout::eGeneral;
    case wind::ImageUsage::COLOR_ATTACHMENT:
        return vk::ImageLayout::eColorAttachmentOptimal;
    case wind::ImageUsage::DEPTH_SPENCIL_ATTACHMENT:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case wind::ImageUsage::INPUT_ATTACHMENT:
        return vk::ImageLayout::eAttachmentOptimalKHR; // TODO: is it ok?
    case wind::ImageUsage::FRAGMENT_SHADING_RATE_ATTACHMENT:
        return vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR;
    default:
        assert(false);
        return vk::ImageLayout::eUndefined;
    }
}

vk::AccessFlags ImageUsageToAccessFlags(ImageUsage::Bits layout) {
    switch (layout) {
    case ImageUsage::UNKNOWN:
        return vk::AccessFlags{};
    case ImageUsage::TRANSFER_SOURCE:
        return vk::AccessFlagBits::eTransferRead;
    case ImageUsage::TRANSFER_DESTINATION:
        return vk::AccessFlagBits::eTransferWrite;
    case ImageUsage::SHADER_READ:
        return vk::AccessFlagBits::eShaderRead;
    case ImageUsage::STORAGE:
        return vk::AccessFlagBits::eShaderRead |
               vk::AccessFlagBits::eShaderWrite; // TODO: what if storage is not read or write?
    case ImageUsage::COLOR_ATTACHMENT:
        return vk::AccessFlagBits::eColorAttachmentWrite;
    case ImageUsage::DEPTH_SPENCIL_ATTACHMENT:
        return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case ImageUsage::INPUT_ATTACHMENT:
        return vk::AccessFlagBits::eInputAttachmentRead;
    case ImageUsage::FRAGMENT_SHADING_RATE_ATTACHMENT:
        return vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
    default:
        assert(false);
        return vk::AccessFlags{};
    }
}

vk::PipelineStageFlags ImageUsageToPipelineStage(ImageUsage::Bits layout) {
    switch (layout) {
    case ImageUsage::UNKNOWN:
        return vk::PipelineStageFlagBits::eTopOfPipe;
    case ImageUsage::TRANSFER_SOURCE:
        return vk::PipelineStageFlagBits::eTransfer;
    case ImageUsage::TRANSFER_DESTINATION:
        return vk::PipelineStageFlagBits::eTransfer;
    case ImageUsage::SHADER_READ:
        return vk::PipelineStageFlagBits::eFragmentShader; // TODO: whats for vertex shader reads?
    case ImageUsage::STORAGE:
        return vk::PipelineStageFlagBits::eFragmentShader; // TODO: whats for vertex shader reads?
    case ImageUsage::COLOR_ATTACHMENT:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case ImageUsage::DEPTH_SPENCIL_ATTACHMENT:
        return vk::PipelineStageFlagBits::eEarlyFragmentTests; // TODO: whats for late fragment
                                                               // test?
    case ImageUsage::INPUT_ATTACHMENT:
        return vk::PipelineStageFlagBits::eFragmentShader; // TODO: check if at least works
    case ImageUsage::FRAGMENT_SHADING_RATE_ATTACHMENT:
        return vk::PipelineStageFlagBits::eFragmentShadingRateAttachmentKHR;
    default:
        assert(false);
        return vk::PipelineStageFlags{};
    }
}

vk::ImageSubresourceLayers GetDefaultImageSubresourceLayers(const Image& image) {
    auto subresourceRange = GetDefaultImageSubresourceRange(image);
    return vk::ImageSubresourceLayers{subresourceRange.aspectMask, subresourceRange.baseMipLevel,
                                      subresourceRange.baseArrayLayer, subresourceRange.layerCount};
}

vk::ImageSubresourceLayers GetDefaultImageSubresourceLayers(const Image& image, uint32_t mipLevel,
                                                            uint32_t layer) {
    return vk::ImageSubresourceLayers{ImageFormatToImageAspect(image.GetFormat()), mipLevel, layer,
                                      1};
}

vk::ImageSubresourceRange GetDefaultImageSubresourceRange(const Image& image) {
    return vk::ImageSubresourceRange{ImageFormatToImageAspect(image.GetFormat()),
                                     0, // base mip level
                                     image.GetMipLevelCount(),
                                     0, // base layer
                                     image.GetLayerCount()};
}

uint32_t CalculateImageMipLevelCount(ImageOptions::Value options, uint32_t width, uint32_t height) {
    if (options & ImageOptions::MIPMAPS)
        return (uint32_t)std::floor(std::log2(std::max(width, height))) + 1;
    else
        return 1;
}

uint32_t CalculateImageLayerCount(ImageOptions::Value options) {
    if (options & ImageOptions::CUBEMAP) return 6;
    else
        return 1;
}

void Image::Destroy() {
    auto& device = RenderBackend::GetInstance().GetDevice();
    device.waitIdle();
    if ((bool)m_handle) {
        if ((bool)m_allocation) DeallocateImage(m_handle, m_allocation);

        RenderBackend::GetInstance().GetDevice().destroyImageView(m_defaultImageViews.nativeView);
        if ((bool)m_defaultImageViews.depthOnlyView)
            RenderBackend::GetInstance().GetDevice().destroyImageView(
                m_defaultImageViews.depthOnlyView);
        if ((bool)m_defaultImageViews.stencilOnlyView)
            RenderBackend::GetInstance().GetDevice().destroyImageView(
                m_defaultImageViews.stencilOnlyView);

        for (auto& imageViewLayer : m_cubemapImageViews) {
            RenderBackend::GetInstance().GetDevice().destroyImageView(imageViewLayer.nativeView);
            if ((bool)imageViewLayer.depthOnlyView)
                RenderBackend::GetInstance().GetDevice().destroyImageView(
                    imageViewLayer.depthOnlyView);
            if ((bool)imageViewLayer.stencilOnlyView)
                RenderBackend::GetInstance().GetDevice().destroyImageView(
                    imageViewLayer.stencilOnlyView);
        }

        m_handle            = vk::Image{};
        m_defaultImageViews = {};
        m_cubemapImageViews.clear();
        m_extent        = vk::Extent2D{0u, 0u};
        m_mipLevelCount = 1;
        m_layerCount    = 1;
    }
}

void Image::InitViews(const vk::Image& image, vk::Format format) {
    m_handle = image;
    m_format = format;

    auto& backend          = RenderBackend::GetInstance();
    auto  subresourceRange = GetDefaultImageSubresourceRange(*this);

    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.setImage(m_handle)
        .setViewType(GetImageViewType(*this))
        .setFormat(format)
        .setComponents(
            vk::ComponentMapping{vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity})
        .setSubresourceRange(subresourceRange);

    auto nativeSubresourceRange = GetDefaultImageSubresourceRange(*this);
    imageViewCreateInfo.setSubresourceRange(nativeSubresourceRange);
    m_defaultImageViews.nativeView = backend.GetDevice().createImageView(imageViewCreateInfo);

    auto depthSubresourceRange = GetDefaultImageSubresourceRange(*this);
    depthSubresourceRange.setAspectMask(depthSubresourceRange.aspectMask &
                                        vk::ImageAspectFlagBits::eDepth);
    if (depthSubresourceRange.aspectMask != vk::ImageAspectFlags{}) {
        imageViewCreateInfo.setSubresourceRange(depthSubresourceRange);
        m_defaultImageViews.depthOnlyView =
            RenderBackend::GetInstance().GetDevice().createImageView(imageViewCreateInfo);
    }

    auto stencilSubresourceRange = GetDefaultImageSubresourceRange(*this);
    stencilSubresourceRange.setAspectMask(stencilSubresourceRange.aspectMask &
                                          vk::ImageAspectFlagBits::eStencil);
    if (stencilSubresourceRange.aspectMask != vk::ImageAspectFlags{}) {
        imageViewCreateInfo.setSubresourceRange(stencilSubresourceRange);
        m_defaultImageViews.stencilOnlyView =
            RenderBackend::GetInstance().GetDevice().createImageView(imageViewCreateInfo);
    }

    if (m_layerCount > 1) {
        m_cubemapImageViews.resize(m_layerCount);
        imageViewCreateInfo.setViewType(vk::ImageViewType::e2DArray);
    }
    uint32_t layer = 0;
    for (auto& imageViewLayer : m_cubemapImageViews) {
        auto nativeSubresourceRange = GetDefaultImageSubresourceRange(*this);
        nativeSubresourceRange.setBaseArrayLayer(layer).setLayerCount(VK_REMAINING_ARRAY_LAYERS);
        imageViewCreateInfo.setSubresourceRange(nativeSubresourceRange);
        imageViewLayer.nativeView = backend.GetDevice().createImageView(imageViewCreateInfo);

        auto depthSubresourceRange = GetDefaultImageSubresourceRange(*this);
        depthSubresourceRange
            .setAspectMask(depthSubresourceRange.aspectMask & vk::ImageAspectFlagBits::eDepth)
            .setBaseArrayLayer(layer)
            .setLayerCount(VK_REMAINING_ARRAY_LAYERS);
        if (depthSubresourceRange.aspectMask != vk::ImageAspectFlags{}) {
            imageViewCreateInfo.setSubresourceRange(depthSubresourceRange);
            imageViewLayer.depthOnlyView =
                RenderBackend::GetInstance().GetDevice().createImageView(imageViewCreateInfo);
        }

        auto stencilSubresourceRange = GetDefaultImageSubresourceRange(*this);
        stencilSubresourceRange
            .setAspectMask(stencilSubresourceRange.aspectMask & vk::ImageAspectFlagBits::eStencil)
            .setBaseArrayLayer(layer)
            .setLayerCount(VK_REMAINING_ARRAY_LAYERS);
        if (stencilSubresourceRange.aspectMask != vk::ImageAspectFlags{}) {
            imageViewCreateInfo.setSubresourceRange(stencilSubresourceRange);
            imageViewLayer.stencilOnlyView =
                RenderBackend::GetInstance().GetDevice().createImageView(imageViewCreateInfo);
        }

        layer++;
    }
}

Image::Image(uint32_t width, uint32_t height, vk::Format format, ImageUsage::Value usage,
             MemoryUsage memoryUsage, ImageOptions::Value options) {
    Init(width, height, format, usage, memoryUsage, options);
}

Image::Image(vk::Image image, uint32_t width, uint32_t height, vk::Format format) {
    m_extent     = vk::Extent2D{width, height};
    m_allocation = {}; // image is external resource
    InitViews(image, format);
}

Image::Image(Image&& other) noexcept {
    m_handle            = other.m_handle;
    m_defaultImageViews = other.m_defaultImageViews;
    m_cubemapImageViews = std::move(other.m_cubemapImageViews);
    m_extent            = other.m_extent;
    m_format            = other.m_format;
    m_allocation        = other.m_allocation;
    m_mipLevelCount     = other.m_mipLevelCount;
    m_layerCount        = other.m_layerCount;

    other.m_handle            = vk::Image{};
    other.m_defaultImageViews = {};
    other.m_cubemapImageViews.clear();
    other.m_extent        = vk::Extent2D{0u, 0u};
    other.m_format        = vk::Format::eUndefined;
    other.m_allocation    = {};
    other.m_mipLevelCount = 1;
    other.m_layerCount    = 1;
}

Image& Image::operator=(Image&& other) noexcept {
    Destroy();

    m_handle            = other.m_handle;
    m_defaultImageViews = other.m_defaultImageViews;
    m_cubemapImageViews = std::move(other.m_cubemapImageViews);
    m_extent            = other.m_extent;
    m_format            = other.m_format;
    m_allocation        = other.m_allocation;
    m_mipLevelCount     = other.m_mipLevelCount;
    m_layerCount        = other.m_layerCount;

    other.m_handle            = vk::Image{};
    other.m_defaultImageViews = {};
    other.m_cubemapImageViews.clear();
    other.m_extent        = vk::Extent2D{0u, 0u};
    other.m_format        = vk::Format::eUndefined;
    other.m_allocation    = {};
    other.m_mipLevelCount = 1;
    other.m_layerCount    = 1;

    return *this;
}

Image::~Image() { Destroy(); }

void Image::Init(uint32_t width, uint32_t height, vk::Format format, ImageUsage::Value usage,
                 MemoryUsage memoryUsage, ImageOptions::Value options) {
    m_mipLevelCount = CalculateImageMipLevelCount(options, width, height);
    m_layerCount    = CalculateImageLayerCount(options);
    
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setImageType(vk::ImageType::e2D)
        .setFormat(format)
        .setExtent(vk::Extent3D{width, height, 1})
        .setSamples(vk::SampleCountFlagBits::e1)
        .setMipLevels(GetMipLevelCount())
        .setArrayLayers(m_layerCount)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage((vk::ImageUsageFlags)usage)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    if (options & ImageOptions::CUBEMAP)
        imageCreateInfo.setFlags(vk::ImageCreateFlagBits::eCubeCompatible);

    m_extent     = vk::Extent2D{(uint32_t)width, (uint32_t)height};
    m_allocation = AllocateImage(imageCreateInfo, memoryUsage, &m_handle);
    InitViews(m_handle, format);
}

vk::ImageView Image::GetNativeView(ImageView view) const {
    switch (view) {
    case wind::ImageView::NATIVE:
        return m_defaultImageViews.nativeView;
    case wind::ImageView::DEPTH_ONLY:
        return m_defaultImageViews.depthOnlyView;
    case wind::ImageView::STENCIL_ONLY:
        return m_defaultImageViews.stencilOnlyView;
    default:
        assert(false);
        return m_defaultImageViews.nativeView;
    }
}

vk::ImageView Image::GetNativeView(ImageView view, uint32_t layer) const {
    if (m_layerCount == 1) return GetNativeView(view);

    switch (view) {
    case wind::ImageView::NATIVE:
        return m_cubemapImageViews[layer].nativeView;
    case wind::ImageView::DEPTH_ONLY:
        return m_cubemapImageViews[layer].depthOnlyView;
    case wind::ImageView::STENCIL_ONLY:
        return m_cubemapImageViews[layer].stencilOnlyView;
    default:
        assert(false);
        return m_cubemapImageViews[layer].nativeView;
    }
}

uint32_t Image::GetMipLevelWidth(uint32_t mipLevel) const {
    auto safeWidth = std::max(GetWidth(), 1u << mipLevel);
    return safeWidth >> mipLevel;
}

uint32_t Image::GetMipLevelHeight(uint32_t mipLevel) const {
    auto safeHeight = std::max(GetHeight(), 1u << mipLevel);
    return safeHeight >> mipLevel;
}

} // namespace wind