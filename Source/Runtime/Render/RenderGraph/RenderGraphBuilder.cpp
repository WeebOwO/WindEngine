#include "RenderGraphBuilder.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
    void RenderGraphBuilder::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
        m_renderGraph->AddRenderPass(passName, setupFunc);
    }
    
    std::shared_ptr<RDGRenderTarget> RenderGraphBuilder::CreateRDGRenderTarget(const std::string& name, uint32_t width, uint32_t height) {
        RDGRenderTarget renderTarget;
        return nullptr;
    }

    void RenderGraphBuilder::Setup(SceneView* renderScene) {
        m_renderGraph->Setup(renderScene);
    }
    
    void RenderGraphBuilder::Compile() {
        for(auto& pass : m_renderGraph->m_passNodes) {
            pass->ConstructResource(*this);
            pass->graphicsShader->FinishShaderBinding();
        }
    }

    void RenderGraphBuilder::Exec() {
        m_renderGraph->Exec();
    }
    
    std::shared_ptr<Buffer> RenderGraphBuilder::CreateRDGBuffer(const std::string& resourceName, const BufferDesc& bufferDesc) {
        std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(bufferDesc.byteSize, bufferDesc.usage, bufferDesc.memoryUsage);
        auto* node = new ResourceNode();

        node->resourceName = resourceName;
        node->bufferHandle = buffer;
        node->resoueceType = RenderResoueceType::Buffer;
        
        m_renderGraph->AddResourceNode(resourceName, node);
        return buffer;
    }

    void RenderGraphBuilder::SetBackBufferName(std::string_view backBufferName) {
        m_renderGraph->SetBackBufferName(backBufferName);
    }

    std::shared_ptr<Image> RenderGraphBuilder::CreateRDGTexture(const std::string& resourceName, const TextureDesc& textureDesc) {
        std::shared_ptr<Image> texture = std::make_shared<Image>(textureDesc.width, textureDesc.height, textureDesc.format, textureDesc.usage, textureDesc.memoryUsage, textureDesc.options);
        auto* node = new ResourceNode();
        
        node->resourceName = resourceName;
        node->imageHandle = texture;
        node->resoueceType = RenderResoueceType::Image;

        m_renderGraph->AddResourceNode(resourceName, node);
        return texture;
    }
}