#include "PassRendering.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

namespace wind {
    void AddBloomSetupPass(RenderGraphBuilder& graphBuilder) {
        const auto [width, height] = RenderBackend::GetInstance().GetSurfaceExtent();
        TextureDesc SetupTexture {width, height, vk::SampleCountFlagBits::e1, vk::Format::eR16G16B16A16Sfloat};
    }
}