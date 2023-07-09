#include "Runtime/Render/RenderGraph/RenderGraphBuilder.h"
#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/Shader.h"
#include "Runtime/Scene/SceneView.h"

namespace wind {
    // RenderOrder SkyBox -> ForwardBasePass
    void AddForwardBasePass(RenderGraphBuilder& graphBuilder);
    void AddSkyboxPass(RenderGraphBuilder& graphBuilder);
}