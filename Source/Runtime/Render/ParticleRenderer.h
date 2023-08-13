#pragma once

#include "Runtime/Render/Renderer.h"

namespace wind {
class ParticleRenderer : Renderer {
public:
    void Init() override;
    void Render(Scene& scene) override;
    void InitView(Scene& scene) override;
};
} // namespace wind