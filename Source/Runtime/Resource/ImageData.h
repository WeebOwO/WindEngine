#pragma once

#include <vector>

namespace wind {
struct ImageData {
    std::vector<uint8_t>              ByteData;
    Format                            ImageFormat = Format::UNDEFINED;
    uint32_t                          Width       = 0;
    uint32_t                          Height      = 0;
    std::vector<std::vector<uint8_t>> MipLevels;
};
} // namespace wind