#pragma once

#include "Runtime/Render/RenderGraph.h"

#include "Runtime/Render/RHI/RenderPassBase.h"

namespace wind {
    class BasePass : public RenderPassBase {
    public:
        BasePass(std::string_view name, RenderGraph& graph) : RenderPassBase(name, graph) {}
        void Setup() override;
        void Exec() override;
    private:
        enum class DescriptorLayoutGroups : uint8_t {
            viewUniform = 0,
            descriptorLayoutGroupsCnt
        };
    };
}