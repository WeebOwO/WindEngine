#pragma once

#include <string>

#include "Runtime/Resource/ImageData.h"
#include "Runtime/Render/RHI/CommandBuffer.h"

namespace wind {
class ImageLoader {
public:
    static ImageData   LoadImageDataFromFile(const std::string& filepath);
    static CubemapData LoadCubemapDataFromFile(const std::string& filepath);
    static void FillImage(CommandBuffer& commandBuffer, Image& image, const ImageData& imageData, ImageOptions::Value options);
    static void FillImage(Image& image, CommandBuffer& cmdBuffer, const std::string& filepath, ImageOptions::Value options);
    static void LoadCubemap(Image& image, const std::string& filepath);
};
} // namespace wind