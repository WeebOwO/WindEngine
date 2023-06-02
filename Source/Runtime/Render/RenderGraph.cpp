#include "RenderGraph.h"

#include "Runtime/Render/RHI/RenderPassBase.h"

namespace wind {
    void RenderGraph::AddRenderPass(std::unique_ptr<RenderPassBase> renderPass) {
        m_renderPassList.push_back(std::move(renderPass));
    }

    void RenderGraph::Setup() {
        for(const auto& pass : m_renderPassList) {
            pass->Setup();
        }
    }   

    void RenderGraph::Exec() {
        for(const auto& pass : m_renderPassList) {
            pass->Exec();
        }
    }

    void RenderGraph::DeclareAttachmentOutput(std::string_view resourseName, std::shared_ptr<Image> attachment) {
        m_graphAttachments[std::string(resourseName)] = attachment;
    }
    
}