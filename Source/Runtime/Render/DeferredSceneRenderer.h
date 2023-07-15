#pragma once

#include "Runtime/Render/Renderer.h"

namespace wind {
class DeferedSceneRenderer final : public Renderer {
public:
    DeferedSceneRenderer();
    void Render(Scene& scene) override;
protected:
    void Init() override;
    void InitView(Scene& scene) override;
};
} // namespace wind