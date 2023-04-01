#pragma once

#include <list>
#include <memory>
#include <string_view>

#include "runtime/render/render_pass.h"

namespace wind {
class RenderGraph {
public:
    template <typename SetupFunc, typename ExecFunc>
    void AddRenderPass(std::string_view renderPassName, const SetupFunc& setupFunc, const ExecFunc& execFunc) {
        auto renderPass = std::make_unique<RenderPassBase>(renderPassName, setupFunc, execFunc);
        renderPasses.push_back(renderPass);
    }

    void AddRenderPass(std::unique_ptr<RenderPassBase> renderPass) {
        renderPasses.push_back(renderPass);
    }

    void Setup() {
        for(auto& rp : renderPasses) {
            rp->Setup();
        }
    }

    void Run() {
        for(auto& rp : renderPasses) {
            rp->Exec();
        }
    }
    
private:
    std::list<std::unique_ptr<RenderPassBase>> renderPasses;
};
} // namespace wind