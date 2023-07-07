#include "Material.h"

namespace wind {
Material::Material() {
    albedoTexture    = std::make_shared<Image>();
    normalTexture    = std::make_shared<Image>();
    roughnessTexture = std::make_shared<Image>();
    metallicTexture  = std::make_shared<Image>();
}
} // namespace wind