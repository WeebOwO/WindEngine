#pragma once

#include <string>

#include "Runtime/Render/RHI/CommandBuffer.h"
#include "Runtime/Resource/ImageData.h"

namespace wind {
class ImageLoader {
public:
    static ImageData   LoadImageDataFromFile(const std::string& filepath, Format format);
    static CubemapData LoadCubemapDataFromFile(const std::string& filepath, Format format);
    static void FillImage(CommandBuffer& commandBuffer, Image& image, const ImageData& imageData,
                          ImageOptions::Value options);
    static void FillImage(Image& image, Format format, CommandBuffer& cmdBuffer,
                          const std::string& filepath, ImageOptions::Value options);
    static void FillImage(Image& image, Format format, const std::string& filepath,
                          ImageOptions::Value options);
    static void FillImage(Image& image, ImageData& imageData, ImageOptions::Value options);

    static void LoadCubemap(Image& image, Format format, const std::string& filepath);
};
} // namespace wind