#pragma once

#include <vector>

#include "Runtime/Render/RHI/Image.h"

namespace wind {

struct ImageData {
    std::vector<uint8_t>              ByteData;
    Format                            ImageFormat = Format::UNDEFINED;
    uint32_t                          Width       = 0;
    uint32_t                          Height      = 0;
    std::vector<std::vector<uint8_t>> MipLevels;
    bool                              isHdr = false;
};

struct CubemapData {
    std::array<std::vector<uint8_t>, 6> Faces;
    Format                              FaceFormat = Format::UNDEFINED;
    uint32_t                            FaceWidth  = 0;
    uint32_t                            FaceHeight = 0;
};

} // namespace wind