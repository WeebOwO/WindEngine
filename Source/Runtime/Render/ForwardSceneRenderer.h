#include "Renderer.h"

namespace wind {
class ForwardRenderer : public Renderer {
public:
    ForwardRenderer();
    void Render(Scene& scene) override;
    void Init() override;
};
} // namespace wind