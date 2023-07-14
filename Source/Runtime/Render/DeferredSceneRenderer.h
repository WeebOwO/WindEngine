#pragma once

#include "Runtime/Render/Renderer.h"

namespace wind {
class DeferedSceneRenderer : public Renderer {
public:
    DeferedSceneRenderer();
    void Init() override;
    void InitView(Scene& scene) override;
    void Render(Scene& scene) override;
};
} // namespace wind