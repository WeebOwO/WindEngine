#include "Renderer.h"

namespace wind {
class ForwardRenderer : public Renderer {
public:
    ForwardRenderer();
    void Render(Scene& scene) override;
    void Init() override;
    void InitView(Scene& scene) override;
private:
    void AddForwardBasePass(RenderGraphBuilder& graphBuilder);
};
} // namespace wind