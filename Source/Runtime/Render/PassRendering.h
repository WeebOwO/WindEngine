#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Scene/SceneView.h"

// Actully this is bad header design which will increase complie time
// But it's good for now!
namespace wind {
    // Forward renderer
    void AddForwardBasePass(RenderGraphBuilder& graphBuilder);
    void AddSkyboxPass(RenderGraphBuilder& graphBuilder);
    // For defer shading usage
    void LightGridComputePass(RenderGraphBuilder& graphBuilder);
    void AddDeferedBasePass(RenderGraphBuilder& graphBuilder);
    void AddShadowPass(RenderGraphBuilder& graphBuilder);
    // Postprocess part
    void AddBloomSetupPass(RenderGraphBuilder& graphBuilder);
    void AddSSAOPass(RenderGraphBuilder& graphBuilder);
    void AddToneMappingCombinePass(RenderGraphBuilder& graphBuilder, uint32_t swapChainImageIndex);
}