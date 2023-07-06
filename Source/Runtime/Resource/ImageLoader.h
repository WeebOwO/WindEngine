#pragma once

#include <string>

#include "Runtime/Resource/ImageData.h"

namespace wind {
class ImageLoader {
public:
    static ImageData   LoadImageFromFile(const std::string& filepath);
    static CubemapData LoadCubemapImageFromFile(const std::string& filepath);
};
} // namespace wind