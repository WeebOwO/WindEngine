#include "RenderGraphBuilder.h"

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RenderGraph/Node.h"

namespace wind {
    void RenderGraphBuilder::AddRenderPass(std::string_view passName, PassSetupFunc setupFunc) {
        m_renderGraph->AddRenderPass(passName, setupFunc);
    }
    
    std::shared_ptr<RDGRenderTarget> RenderGraphBuilder::CreateRDGRenderTarget(const std::string& name, uint32_t width, uint32_t height) {
        // todo
        return nullptr;
    }

    void RenderGraphBuilder::Setup(SceneView* renderScene) {
        m_renderGraph->Setup(renderScene);
    }
    
    void RenderGraphBuilder::Compile() {
        for(auto& pass : m_renderGraph->m_passNodes) {
            pass->ConstructResource(*this);
        }
    }

    void RenderGraphBuilder::Exec() {
        m_renderGraph->Exec();
    }
    
    std::shared_ptr<Buffer> RenderGraphBuilder::TryCreateRDGBuffer(const std::string& resourceName, const BufferDesc& bufferDesc) {
        std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(bufferDesc.byteSize, bufferDesc.usage, bufferDesc.memoryUsage);
        auto node = std::make_shared<ResourceNode>();

        node->resourceName = resourceName;
        node->bufferHandle = buffer;
        node->resoueceType = RenderResoueceType::Buffer;
        
        m_renderGraph->AddResourceNode(resourceName, node);
        return buffer;
    }

    void RenderGraphBuilder::SetBackBufferName(std::string_view backBufferName) {
        m_renderGraph->SetBackBufferName(backBufferName);
    }

    std::shared_ptr<Image> RenderGraphBuilder::TryCreateRDGTexture(const std::string& resourceName, const TextureDesc& textureDesc) {
        if(m_renderGraph->Contains(resourceName)) {
            return m_renderGraph->GetImageResourceByName(resourceName);
        }
        std::shared_ptr<Image> texture = std::make_shared<Image>(textureDesc.width, textureDesc.height, textureDesc.format, textureDesc.usage, textureDesc.memoryUsage, textureDesc.options);
        auto node = std::make_shared<ResourceNode>();
        
        node->resourceName = resourceName;
        node->imageHandle = texture;
        node->resoueceType = RenderResoueceType::Image;

        m_renderGraph->AddResourceNode(resourceName, node);
        return texture;
    }

    void RenderGraphBuilder::ImportResource(const std::string& resourceName, std::shared_ptr<Image> image) {
        auto node = std::make_shared<ResourceNode>();
        node->resourceName = resourceName;
        node->external = true;
        node->imageHandle = image;
        node->resoueceType = RenderResoueceType::Image;
        m_renderGraph->AddResourceNode(resourceName, node);
    }

    void RenderGraphBuilder::ImportSceneTextures(const SceneTexture& sceneTexture) {
        for(const auto& [name, image] : sceneTexture.SceneTextures) {
            ImportResource(name, image);
        }
    }
}