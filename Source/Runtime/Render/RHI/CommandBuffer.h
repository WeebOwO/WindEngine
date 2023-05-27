#pragma once

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/Rhi/Buffer.h"
#include "Runtime/Render/Rhi/Image.h"

namespace wind {
struct PassNode;
struct ResourceNode;

struct Rect2D {
    int32_t  offsetWidth  = 0;
    int32_t  offsetHeight = 0;
    uint32_t width        = 0;
    uint32_t height       = 0;
};

struct Viewport {
    float offsetWidth  = 0.0f;
    float offsetHeight = 0.0f;
    float width        = 0.0f;
    float height       = 0.0f;
    float minDepth     = 0.0f;
    float maxDepth     = 0.0f;
};

struct ClearColor {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

struct ClearDepthStencil {
    float    depth   = 1.0f;
    uint32_t stencil = 0;
};

enum class BlitFilter {
    NEAREST = 0,
    LINEAR,
    CUBIC,
};

struct ImageInfo {
    ImageReference   resource;
    ImageUsage::Bits usage    = ImageUsage::UNKNOWN;
    uint32_t         mipLevel = 0;
    uint32_t         layer    = 0;
};

struct BufferInfo {
    BufferReference resource;
    uint32_t        offset = 0;
};

class CommandBuffer {
public:
    CommandBuffer(vk::CommandBuffer commandBuffer) : m_handle(std::move(commandBuffer)) {}

    [[nodiscard]] const auto& GetNativeHandle() const { return m_handle; }
    void                      Begin();
    void                      End();

    void Draw(uint32_t vertexCount, uint32_t instanceCount);

    void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
              uint32_t firstInstance);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount);
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                     uint32_t vertexOffset, uint32_t firstInstance);
    void BindIndexBufferUInt32(const Buffer& indexBuffer);
    void BindIndexBufferUInt16(const Buffer& indexBuffer);

    void SetViewport(const Viewport& viewport);
    void SetScissor(const Rect2D& scissor);
    void SetRenderArea(const Image& image);

    void Dispatch(uint32_t x, uint32_t y, uint32_t z);

    void BeginRenderPass(const PassNode& passNode, const ResourceNode& resourceNode);
    void EndRenderPass();
    
    void CopyImage(const ImageInfo& source, const ImageInfo& distance);
    void CopyBufferToImage(const BufferInfo& source, const ImageInfo& distance);
    void CopyImageToBuffer(const ImageInfo& source, const BufferInfo& distance);
    void CopyBuffer(const BufferInfo& source, const BufferInfo& distance, size_t byteSize);

    void BlitImage(const Image& source, ImageUsage::Bits sourceUsage, const Image& distance,
                   ImageUsage::Bits distanceUsage, BlitFilter filter);
    void GenerateMipLevels(const Image& image, ImageUsage::Bits initialUsage, BlitFilter filter);

    void TransferLayout(const Image& image, ImageUsage::Bits oldLayout, ImageUsage::Bits newLayout);
    void TransferLayout(std::span<ImageReference> images, ImageUsage::Bits oldLayout,
                        ImageUsage::Bits newLayout);
    void TransferLayout(std::span<Image> images, ImageUsage::Bits oldLayout,
                        ImageUsage::Bits newLayout);

    void BindPipeline(const PassNode& passNode);
     
    template <typename... Buffers> void BindVertexBuffers(const Buffers&... vertexBuffers) {
        constexpr size_t BufferCount          = sizeof...(Buffers);
        std::array       buffers              = {vertexBuffers.GetNativeHandle()...};
        uint64_t         offsets[BufferCount] = {0};
        GetNativeHandle().bindVertexBuffers(0, BufferCount, buffers.data(), offsets);
    }

private:
    vk::CommandBuffer m_handle;
};
} // namespace wind