#include "runtime/resource/texture.h"
#include "runtime/base/utils.h"

#include <stdexcept>
#include <vcruntime_string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "runtime/base/macro.h"

namespace wind {
    Texture::Texture(std::string_view filepath) {
        auto& device = utils::GetRHIDevice();
        int width, height, channels;
        unsigned char* pixels;

        vk::DeviceSize imageSize;
        vk::Format imageFormat;
        // some align problem here
        if(stbi_is_hdr(filepath.data())) {
            float* tempPixels = stbi_loadf(filepath.data(), &width, &height, &channels, STBI_rgb_alpha);
            pixels = reinterpret_cast<unsigned char*>(tempPixels);
            imageSize = width * height * 4 * sizeof(float);
            imageFormat = vk::Format::eR32G32B32A32Sfloat;
        } else {
            pixels = stbi_load(filepath.data(), &width, &height, &channels, STBI_rgb_alpha);
            imageSize = width * height * 4 * sizeof(unsigned char);
            imageFormat = vk::Format::eR8G8B8A8Srgb;
        }

        if(!pixels) {
            WIND_CORE_ERROR("Failed to create texture!");
        }

        // Create staging buffer
        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        utils::CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        void* data = device.mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, imageSize);
        device.unmapMemory(stagingBufferMemory);

        stbi_image_free(pixels);

        // Create texure image 
        vk::ImageCreateInfo createInfo;
        createInfo.setImageType(vk::ImageType::e2D)
                  .setArrayLayers(1)
                  .setMipLevels(1)
                  .setExtent({uint32_t(width), uint32_t(height), 1})
                  .setFormat(imageFormat)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                  .setSamples(vk::SampleCountFlagBits::e1);

        image = device.createImage(createInfo);

        auto memRequirements = device.getImageMemoryRequirements(image);

        vk::MemoryAllocateInfo allocateInfo;
        
        allocateInfo.setMemoryTypeIndex(utils::FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal))
                    .setAllocationSize(memRequirements.size);

        memory = device.allocateMemory(allocateInfo);
        device.bindImageMemory(image, memory, 0);

        utils::TransitionImageLayout(image, imageFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        utils::CopyBufferToImage(stagingBuffer, image, width, height);
        utils::TransitionImageLayout(image, imageFormat, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        device.destroyBuffer(stagingBuffer);
        device.freeMemory(stagingBufferMemory);

        vk::ImageViewCreateInfo viewCreateInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange  range;
        
        range.setAspectMask(vk::ImageAspectFlagBits::eColor)
             .setBaseArrayLayer(0)
             .setLayerCount(1)
             .setLevelCount(1)
             .setBaseMipLevel(0);

        viewCreateInfo.setImage(image)
                      .setViewType(vk::ImageViewType::e2D)
                      .setComponents(mapping)
                      .setFormat(imageFormat)
                      .setSubresourceRange(range);

        view = device.createImageView(viewCreateInfo);
    }
    
    Texture::~Texture() {
        auto& device = utils::GetRHIDevice();

        device.destroyImageView(view);
        device.freeMemory(memory);
        device.destroyImage(image);
    }
}