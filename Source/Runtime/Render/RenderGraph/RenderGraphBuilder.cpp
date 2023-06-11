#include "RenderGraphBuilder.h"

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
    void RenderGraphBuilder::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
        m_renderGraph->AddRenderPass(passName, setupFunc);
    }
    
    std::shared_ptr<RDGRenderTarget> RenderGraphBuilder::CreateRDGRenderTarget(const std::string& name, uint32_t width, uint32_t height) {
        RDGRenderTarget renderTarget;
        return nullptr;
    }

    std::shared_ptr<Image> RenderGraphBuilder::CreateRDGTexture(const std::string& resourceName, const TextureDesc& textureDesc) {
        std::shared_ptr<Image> texture = std::make_shared<Image>(textureDesc.width, textureDesc.height, textureDesc.format, textureDesc.usage, textureDesc.memoryUsage, textureDesc.options);
        ResourceNode node;
        node.resourceName = resourceName;
        node.resourceHandle = texture;
        node.resoueceType = RenderResoueceType::Image;

        m_renderGraph->AddResourceNode(resourceName, node);
        return texture;
    }
}