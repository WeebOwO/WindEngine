#include "Renderer.h"

namespace wind {
class ForwardRenderer final : public Renderer {
public:
    ForwardRenderer();
    void Render(Scene& scene) override;
    void Init() override;
    void InitView(Scene& scene) override;
};
} // namespace wind