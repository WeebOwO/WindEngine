#include "CommandBuffer.h"

#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
vk::Filter BlitFilterToNative(BlitFilter filter) {
    switch (filter) {
    case BlitFilter::NEAREST:
        return vk::Filter::eNearest;
    case BlitFilter::LINEAR:
        return vk::Filter::eLinear;
    case BlitFilter::CUBIC:
        return vk::Filter::eCubicEXT;
    default:
        assert(false);
        return vk::Filter::eNearest;
    }
}

void CommandBuffer::Begin() {
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_handle.begin(commandBufferBeginInfo);
}

void CommandBuffer::End() { m_handle.end(); }

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount) {
    m_handle.draw(vertexCount, instanceCount, 0, 0);
}

void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
                         uint32_t firstInstance) {
    m_handle.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount) {
    m_handle.drawIndexed(indexCount, instanceCount, 0, 0, 0);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                                uint32_t vertexOffset, uint32_t firstInstance) {
    m_handle.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::BindIndexBufferUInt32(const Buffer& indexBuffer) {
    m_handle.bindIndexBuffer(indexBuffer.GetNativeHandle(), 0, vk::IndexType::eUint32);
}

void CommandBuffer::BindIndexBufferUInt16(const Buffer& indexBuffer) {
    m_handle.bindIndexBuffer(indexBuffer.GetNativeHandle(), 0, vk::IndexType::eUint16);
}

void CommandBuffer::SetViewport(const Viewport& viewport) {
    m_handle.setViewport(0, vk::Viewport{viewport.offsetWidth,
                                         viewport.offsetHeight + viewport.height, viewport.width,
                                         -viewport.height, // inverse viewport height to invert
                                                           // coordinate system
                                         viewport.minDepth, viewport.maxDepth});
}

void CommandBuffer::SetScissor(const Rect2D& scissor) {
    m_handle.setScissor(0, vk::Rect2D{vk::Offset2D{scissor.offsetWidth, scissor.offsetHeight},
                                      vk::Extent2D{scissor.width, scissor.height}});
}

void CommandBuffer::SetRenderArea(const Image& image) {
    SetViewport(
        Viewport{0.0f, 0.0f, (float)image.GetWidth(), (float)image.GetHeight(), 0.0f, 1.0f});
    SetScissor(Rect2D{0, 0, image.GetWidth(), image.GetHeight()});
}


void CommandBuffer::BeginRenderPass(PassNode* passNode) {
    vk::RenderPassBeginInfo beginInfo;
    vk::Rect2D rect = {{}, {passNode->renderRect.width, passNode->renderRect.height}};
    
    std::vector<vk::ClearValue> clearValues = passNode->colorClearValue;
    clearValues.push_back(passNode->depthClearValue);
    beginInfo.setRenderPass(passNode->renderPass)
             .setRenderArea(rect)
             .setClearValueCount(1)
             .setClearValues(clearValues)
             .setFramebuffer(passNode->frameBuffer);
    
    m_handle.beginRenderPass(beginInfo, {});
}

void CommandBuffer::EndRenderPass() {
    m_handle.endRenderPass();
}

void CommandBuffer::BindPipeline(PassNode* passNode) {
    auto renderProcess = passNode->pipelineState->GetPipeline();
    m_handle.bindPipeline(renderProcess.bindPoint, renderProcess.pipeline);
}

void CommandBuffer::Dispatch(uint32_t x, uint32_t y, uint32_t z) { m_handle.dispatch(x, y, z); }

void CommandBuffer::CopyImage(const ImageInfo& source, const ImageInfo& distance) {
    auto sourceRange   = GetDefaultImageSubresourceRange(source.resource.get());
    auto distanceRange = GetDefaultImageSubresourceRange(distance.resource.get());

    std::array<vk::ImageMemoryBarrier, 2> barriers;
    size_t                                barrierCount = 0;

    vk::ImageMemoryBarrier toTransferSrcBarrier;
    toTransferSrcBarrier.setSrcAccessMask(ImageUsageToAccessFlags(source.usage))
        .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
        .setOldLayout(ImageUsageToImageLayout(source.usage))
        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(source.resource.get().GetNativeHandle())
        .setSubresourceRange(sourceRange);

    vk::ImageMemoryBarrier toTransferDstBarrier;
    toTransferDstBarrier.setSrcAccessMask(ImageUsageToAccessFlags(distance.usage))
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setOldLayout(ImageUsageToImageLayout(distance.usage))
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(distance.resource.get().GetNativeHandle())
        .setSubresourceRange(distanceRange);

    if (source.usage != ImageUsage::TRANSFER_SOURCE)
        barriers[barrierCount++] = toTransferSrcBarrier;
    if (distance.usage != ImageUsage::TRANSFER_DESTINATION)
        barriers[barrierCount++] = toTransferDstBarrier;

    if (barrierCount > 0) {
        m_handle.pipelineBarrier(ImageUsageToPipelineStage(source.usage) |
                                     ImageUsageToPipelineStage(distance.usage),
                                 vk::PipelineStageFlagBits::eTransfer, {}, // dependency flags
                                 0, nullptr,                               // memory barriers
                                 0, nullptr,                               // buffer barriers
                                 barrierCount, barriers.data());
    }

    auto sourceLayers =
        GetDefaultImageSubresourceLayers(source.resource.get(), source.mipLevel, source.layer);
    auto distanceLayers = GetDefaultImageSubresourceLayers(distance.resource.get(),
                                                           distance.mipLevel, distance.layer);

    vk::ImageCopy imageCopyInfo;
    imageCopyInfo.setSrcOffset(0)
        .setDstOffset(0)
        .setSrcSubresource(sourceLayers)
        .setDstSubresource(distanceLayers)
        .setExtent(vk::Extent3D{distance.resource.get().GetMipLevelWidth(distance.mipLevel),
                                distance.resource.get().GetMipLevelHeight(distance.mipLevel), 1});

    m_handle.copyImage(source.resource.get().GetNativeHandle(),
                       vk::ImageLayout::eTransferSrcOptimal,
                       distance.resource.get().GetNativeHandle(),
                       vk::ImageLayout::eTransferDstOptimal, imageCopyInfo);
}

void CommandBuffer::CopyImageToBuffer(const ImageInfo& source, const BufferInfo& distance) {
    if (source.usage != ImageUsage::TRANSFER_SOURCE) {
        auto sourceRange = GetDefaultImageSubresourceRange(source.resource.get());

        vk::ImageMemoryBarrier toTransferSrcBarrier;
        toTransferSrcBarrier.setSrcAccessMask(ImageUsageToAccessFlags(source.usage))
            .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
            .setOldLayout(ImageUsageToImageLayout(source.usage))
            .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(source.resource.get().GetNativeHandle())
            .setSubresourceRange(sourceRange);

        m_handle.pipelineBarrier(ImageUsageToPipelineStage(source.usage),
                                 vk::PipelineStageFlagBits::eTransfer, {}, // dependency flags
                                 {},                                       // memory barriers
                                 {},                                       // buffer barriers
                                 toTransferSrcBarrier);
    }

    auto sourceLayers =
        GetDefaultImageSubresourceLayers(source.resource.get(), source.mipLevel, source.layer);

    vk::BufferImageCopy imageToBufferCopyInfo;
    imageToBufferCopyInfo.setBufferOffset(distance.offset)
        .setBufferImageHeight(0)
        .setBufferRowLength(0)
        .setImageSubresource(sourceLayers)
        .setImageOffset(vk::Offset3D{0, 0, 0})
        .setImageExtent(vk::Extent3D{source.resource.get().GetMipLevelWidth(source.mipLevel),
                                     source.resource.get().GetMipLevelHeight(source.mipLevel), 1});

    m_handle.copyImageToBuffer(source.resource.get().GetNativeHandle(),
                               vk::ImageLayout::eTransferSrcOptimal,
                               distance.resource.get().GetNativeHandle(), imageToBufferCopyInfo);
}

void CommandBuffer::DrawIndexIndirect(BufferInfo bufferInfo, uint32_t offset, uint32_t drawcount, uint32_t stride) {
    auto& buffer = bufferInfo.resource.get();
    m_handle.drawIndexedIndirect(buffer.GetNativeHandle(), offset, drawcount, stride);
}

void CommandBuffer::DrawIndirect(BufferInfo bufferInfo, uint32_t offset, uint32_t drawcount, uint32_t stride) {
    auto& buffer = bufferInfo.resource.get();
    m_handle.drawIndirect(buffer.GetNativeHandle(), offset, drawcount, stride);
}

void CommandBuffer::CopyBufferToImage(const BufferInfo& source, const ImageInfo& distance) {
    if (distance.usage != ImageUsage::TRANSFER_DESTINATION) {
        auto distanceRange = GetDefaultImageSubresourceRange(distance.resource.get());

        vk::ImageMemoryBarrier toTransferDstBarrier;
        toTransferDstBarrier.setSrcAccessMask(ImageUsageToAccessFlags(distance.usage))
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setOldLayout(ImageUsageToImageLayout(distance.usage))
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(distance.resource.get().GetNativeHandle())
            .setSubresourceRange(distanceRange);

        m_handle.pipelineBarrier(ImageUsageToPipelineStage(distance.usage),
                                 vk::PipelineStageFlagBits::eTransfer, {}, // dependency flags
                                 {},                                       // memory barriers
                                 {},                                       // buffer barriers
                                 toTransferDstBarrier);
    }

    auto distanceLayers = GetDefaultImageSubresourceLayers(distance.resource.get(),
                                                           distance.mipLevel, distance.layer);

    vk::BufferImageCopy bufferToImageCopyInfo;
    bufferToImageCopyInfo.setBufferOffset(source.offset)
        .setBufferImageHeight(0)
        .setBufferRowLength(0)
        .setImageSubresource(distanceLayers)
        .setImageOffset(vk::Offset3D{0, 0, 0})
        .setImageExtent(vk::Extent3D{distance.resource.get().GetMipLevelWidth(distance.mipLevel),
                                     distance.resource.get().GetMipLevelHeight(distance.mipLevel),
                                     1});

    m_handle.copyBufferToImage(source.resource.get().GetNativeHandle(),
                               distance.resource.get().GetNativeHandle(),
                               vk::ImageLayout::eTransferDstOptimal, bufferToImageCopyInfo);
}

void CommandBuffer::CopyBuffer(const BufferInfo& source, const BufferInfo& distance,
                               size_t byteSize) {
    assert(source.resource.get().GetByteSize() >= source.offset + byteSize);
    assert(distance.resource.get().GetByteSize() >= distance.offset + byteSize);

    vk::BufferCopy bufferCopyInfo;
    bufferCopyInfo.setDstOffset(distance.offset).setSize(byteSize).setSrcOffset(source.offset);

    m_handle.copyBuffer(source.resource.get().GetNativeHandle(),
                        distance.resource.get().GetNativeHandle(), bufferCopyInfo);
}

void CommandBuffer::BlitImage(const Image& source, ImageUsage::Bits sourceUsage,
                              const Image& distance, ImageUsage::Bits distanceUsage,
                              BlitFilter filter) {
    auto sourceRange   = GetDefaultImageSubresourceRange(source);
    auto distanceRange = GetDefaultImageSubresourceRange(distance);

    std::array<vk::ImageMemoryBarrier, 2> barriers;
    size_t                                barrierCount = 0;

    vk::ImageMemoryBarrier toTransferSrcBarrier;
    toTransferSrcBarrier.setSrcAccessMask(ImageUsageToAccessFlags(sourceUsage))
        .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
        .setOldLayout(ImageUsageToImageLayout(sourceUsage))
        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(source.GetNativeHandle())
        .setSubresourceRange(sourceRange);

    vk::ImageMemoryBarrier toTransferDstBarrier;
    toTransferDstBarrier.setSrcAccessMask(ImageUsageToAccessFlags(distanceUsage))
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setOldLayout(ImageUsageToImageLayout(distanceUsage))
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(distance.GetNativeHandle())
        .setSubresourceRange(distanceRange);

    if (sourceUsage != ImageUsage::TRANSFER_SOURCE) barriers[barrierCount++] = toTransferSrcBarrier;
    if (distanceUsage != ImageUsage::TRANSFER_DESTINATION)
        barriers[barrierCount++] = toTransferDstBarrier;

    if (barrierCount > 0) {
        m_handle.pipelineBarrier(ImageUsageToPipelineStage(sourceUsage) |
                                     ImageUsageToPipelineStage(distanceUsage),
                                 vk::PipelineStageFlagBits::eTransfer, {}, // dependency flags
                                 0, nullptr,                               // memory barriers
                                 0, nullptr,                               // buffer barriers
                                 barrierCount, barriers.data());
    }

    auto sourceLayers   = GetDefaultImageSubresourceLayers(source);
    auto distanceLayers = GetDefaultImageSubresourceLayers(distance);

    vk::ImageBlit imageBlitInfo;
    imageBlitInfo
        .setSrcOffsets({vk::Offset3D{0, 0, 0},
                        vk::Offset3D{(int32_t)source.GetWidth(), (int32_t)source.GetHeight(), 1}})
        .setDstOffsets({vk::Offset3D{0, 0, 0}, vk::Offset3D{(int32_t)distance.GetWidth(),
                                                            (int32_t)distance.GetHeight(), 1}})
        .setSrcSubresource(sourceLayers)
        .setDstSubresource(distanceLayers);

    m_handle.blitImage(source.GetNativeHandle(), vk::ImageLayout::eTransferSrcOptimal,
                       distance.GetNativeHandle(), vk::ImageLayout::eTransferDstOptimal,
                       imageBlitInfo, BlitFilterToNative(filter));
}

void CommandBuffer::GenerateMipLevels(const Image& image, ImageUsage::Bits initialUsage,
                                      BlitFilter filter) {
    if (image.GetMipLevelCount() < 2) return;

    auto     sourceRange    = GetDefaultImageSubresourceRange(image);
    auto     distanceRange  = GetDefaultImageSubresourceRange(image);
    auto     sourceLayers   = GetDefaultImageSubresourceLayers(image);
    auto     distanceLayers = GetDefaultImageSubresourceLayers(image);
    auto     sourceUsage    = initialUsage;
    uint32_t sourceWidth    = image.GetWidth();
    uint32_t sourceHeight   = image.GetHeight();
    uint32_t distanceWidth  = image.GetWidth();
    uint32_t distanceHeight = image.GetHeight();

    for (size_t i = 0; i + 1 < image.GetMipLevelCount(); i++) {
        sourceWidth    = distanceWidth;
        sourceHeight   = distanceHeight;
        distanceWidth  = std::max(sourceWidth / 2, 1u);
        distanceHeight = std::max(sourceHeight / 2, 1u);

        sourceLayers.setMipLevel(i);
        sourceRange.setBaseMipLevel(i);
        sourceRange.setLevelCount(1);

        distanceLayers.setMipLevel(i + 1);
        distanceRange.setBaseMipLevel(i + 1);
        distanceRange.setLevelCount(1);

        std::array<vk::ImageMemoryBarrier, 2> imageBarriers;
        imageBarriers[0] // to transfer source
            .setSrcAccessMask(ImageUsageToAccessFlags(sourceUsage))
            .setDstAccessMask(ImageUsageToAccessFlags(ImageUsage::TRANSFER_SOURCE))
            .setOldLayout(ImageUsageToImageLayout(sourceUsage))
            .setNewLayout(ImageUsageToImageLayout(ImageUsage::TRANSFER_SOURCE))
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(image.GetNativeHandle())
            .setSubresourceRange(sourceRange);

        imageBarriers[1] // to transfer distance
            .setSrcAccessMask(ImageUsageToAccessFlags(ImageUsage::UNKNOWN))
            .setDstAccessMask(ImageUsageToAccessFlags(ImageUsage::TRANSFER_DESTINATION))
            .setOldLayout(ImageUsageToImageLayout(ImageUsage::UNKNOWN))
            .setNewLayout(ImageUsageToImageLayout(ImageUsage::TRANSFER_DESTINATION))
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(image.GetNativeHandle())
            .setSubresourceRange(distanceRange);

        m_handle.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                 vk::PipelineStageFlagBits::eTransfer, {}, // dependencies
                                 {},                                       // memory barriers
                                 {},                                       // buffer barriers,
                                 imageBarriers);
        sourceUsage = ImageUsage::TRANSFER_DESTINATION;

        vk::ImageBlit imageBlitInfo;
        imageBlitInfo
            .setSrcOffsets({vk::Offset3D{0, 0, 0},
                            vk::Offset3D{(int32_t)sourceWidth, (int32_t)sourceHeight, 1}})
            .setDstOffsets({vk::Offset3D{0, 0, 0},
                            vk::Offset3D{(int32_t)distanceWidth, (int32_t)distanceHeight, 1}})
            .setSrcSubresource(sourceLayers)
            .setDstSubresource(distanceLayers);

        m_handle.blitImage(image.GetNativeHandle(), vk::ImageLayout::eTransferSrcOptimal,
                           image.GetNativeHandle(), vk::ImageLayout::eTransferDstOptimal,
                           imageBlitInfo, BlitFilterToNative(filter));
    }

    auto mipLevelsSubresourceRange = GetDefaultImageSubresourceRange(image);
    mipLevelsSubresourceRange.setLevelCount(mipLevelsSubresourceRange.levelCount - 1);
    vk::ImageMemoryBarrier mipLevelsTransfer;
    mipLevelsTransfer.setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(image.GetNativeHandle())
        .setSubresourceRange(mipLevelsSubresourceRange);

    m_handle.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eTransfer, {}, // dependecies
                             {},                                       // memory barriers
                             {},                                       // buffer barriers
                             mipLevelsTransfer);
}

static vk::ImageMemoryBarrier GetImageMemoryBarrier(const Image& image, ImageUsage::Bits oldLayout,
                                                    ImageUsage::Bits newLayout) {
    auto                   subresourceRange = GetDefaultImageSubresourceRange(image);
    vk::ImageMemoryBarrier barrier;
    barrier.setSrcAccessMask(ImageUsageToAccessFlags(oldLayout))
        .setDstAccessMask(ImageUsageToAccessFlags(newLayout))
        .setOldLayout(ImageUsageToImageLayout(oldLayout))
        .setNewLayout(ImageUsageToImageLayout(newLayout))
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setImage(image.GetNativeHandle())
        .setSubresourceRange(subresourceRange);

    return barrier;
}

void CommandBuffer::TransferLayout(const Image& image, ImageUsage::Bits oldLayout,
                                   ImageUsage::Bits newLayout) {
    auto barrier = GetImageMemoryBarrier(image, oldLayout, newLayout);

    m_handle.pipelineBarrier(ImageUsageToPipelineStage(oldLayout),
                             ImageUsageToPipelineStage(newLayout), {}, // dependecies
                             {},                                       // memory barriers
                             {},                                       // buffer barriers
                             barrier);
}

void CommandBuffer::TransferLayout(std::span<ImageReference> images, ImageUsage::Bits oldLayout,
                                   ImageUsage::Bits newLayout) {
    std::vector<vk::ImageMemoryBarrier> barriers;
    barriers.reserve(images.size());

    for (const auto& image : images) {
        barriers.push_back(GetImageMemoryBarrier(image.get(), oldLayout, newLayout));
    }

    m_handle.pipelineBarrier(ImageUsageToPipelineStage(oldLayout),
                             ImageUsageToPipelineStage(newLayout), {}, // dependecies
                             {},                                       // memory barriers
                             {},                                       // buffer barriers
                             barriers);
}

void CommandBuffer::TransferLayout(std::span<Image> images, ImageUsage::Bits oldLayout,
                                   ImageUsage::Bits newLayout) {
    std::vector<vk::ImageMemoryBarrier> barriers;
    barriers.reserve(images.size());

    for (const auto& image : images) {
        barriers.push_back(GetImageMemoryBarrier(image, oldLayout, newLayout));
    }

    m_handle.pipelineBarrier(ImageUsageToPipelineStage(oldLayout),
                             ImageUsageToPipelineStage(newLayout), {}, // dependecies
                             {},                                       // memory barriers
                             {},                                       // buffer barriers
                             barriers);
}


void CommandBuffer::BindDescriptorSet(vk::PipelineBindPoint bindPoint, vk::PipelineLayout layout, std::vector<vk::DescriptorSet>& descriptorSets) {
    m_handle.bindDescriptorSets(bindPoint, layout, 0, descriptorSets, {});
}

void CommandBuffer::PushConstants(const PassNode* passNode, const uint8_t* data, size_t size) {
    constexpr size_t MaxPushConstantByteSize = 128;
    std::array<uint8_t, MaxPushConstantByteSize> pushConstants = { };
    std::memcpy(pushConstants.data(), data, size);
    vk::ShaderStageFlags shaderStageFlag;
    if(passNode->passType == PassType::Graphic) {
        shaderStageFlag = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    } else {
        shaderStageFlag = vk::ShaderStageFlagBits::eCompute;
    }
    m_handle.pushConstants(passNode->pipelineState->GetPipeline().pipelineLayout, shaderStageFlag, 0, pushConstants.size(), pushConstants
    .data());
}

} // namespace wind