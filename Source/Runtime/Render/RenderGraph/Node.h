#pragma once

#include <functional>
#include <string>
#include <variant>

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Image.h"
#include "Runtime/Render/RHI/Buffer.h"

namespace wind {
    class RenderGraphRegister;
    class CommandBuffer;

    using PassExecFunc = std::function<void(CommandBuffer, RenderGraphRegister)>;
    using PassSetupFunc = std::function<PassExecFunc()>;
    
    enum class RenderResoueceType : uint8_t {
        Buffer = 0,
        Image
    };

    struct ResourceNode {
        std::string resourceName;
        RenderResoueceType resoueceType;
        std::variant<Image, Buffer> nativeResource;
    };

    struct PassNode {
        std::string passName;
        PassExecFunc passCallback;
    };
}