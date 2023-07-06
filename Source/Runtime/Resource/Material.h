#pragma once

#include <cstdint>

#include "Runtime/Resource/ImageData.h"

namespace wind {

enum class ShadingModel : uint8_t {
    DefaultLit,
    Transparent
};

struct Material {
    ImageData albedoTexture;
    ImageData normalTexture;
    ImageData roughnessTexture;
    ImageData metallicTexture;
    ShadingModel shadingModel {ShadingModel::DefaultLit};
};

} // namespace wind