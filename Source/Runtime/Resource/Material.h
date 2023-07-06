#pragma once

#include <cstdint>

namespace wind {

struct Material {
    uint32_t AlbedoIndex;
    uint32_t NormalIndex;
    uint32_t MetallicRoughnessIndex;
    float    RoughnessScale;
    float    MetallicScale;
    uint32_t Padding[3];
};

} // namespace wind