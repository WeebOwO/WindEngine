#pragma once

#include <cstdint>

#include "Runtime/Resource/ImageData.h"

namespace wind {

enum class ShadingModel : uint8_t { DefaultLit, Transparent };

struct Material {
    Material();
    std::shared_ptr<Image> albedoTexture;
    std::shared_ptr<Image> normalTexture;
    std::shared_ptr<Image> roughnessTexture;
    std::shared_ptr<Image> metallicTexture;
    ShadingModel           shadingModel{ShadingModel::DefaultLit};
};

} // namespace wind