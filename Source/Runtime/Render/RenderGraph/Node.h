#pragma once

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Render/RHI/Image.h"

#include "Runtime/Render/RenderGraph/RenderPass.h"

namespace wind {
class RenderGraphRegister;
class CommandBuffer;

using PassExecFunc  = std::function<void(CommandBuffer, RenderGraphRegister)>;
using PassSetupFunc = std::function<PassExecFunc()>;

enum class RenderResoueceType : uint8_t { Buffer = 0, Image };

struct Node {
    uint32_t inRefCnt  = 0;
    uint32_t outRefcnt = 0;
    bool nevelCull = false;
};

struct ResourceNode : public Node {
    std::string                                                   resourceName;
    RenderResoueceType                                            resoueceType;
    std::variant<std::shared_ptr<Image>, std::shared_ptr<Buffer>> handle;
};

struct PassNode : public Node {
    std::string    passName;
    PassExecFunc   passCallback;
    RenderProcess  renderProcess;

    std::vector<std::string> dependencyResources {};
    std::vector<std::string> outputResources {};

    void Init(const std::string& passName, std::span<std::string_view> inRourecesName,
              std::span<std::string_view> outputResources);
};

} // namespace wind