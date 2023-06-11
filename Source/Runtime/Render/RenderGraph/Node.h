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
class PassNode;
class RenderGraphBuilder;
class TextureDesc;

using PassExecFunc  = std::function<void(CommandBuffer, RenderGraphRegister)>;
using PassSetupFunc = std::function<PassExecFunc(PassNode&)>;

enum class RenderResoueceType : uint8_t { Buffer = 0, Image };

class Node {
protected:
    uint32_t inRefCnt  = 0;
    uint32_t outRefcnt = 0;
    bool     nevelCull = false;
};

class ResourceNode : public Node {
public:
    std::string                                                   resourceName;
    RenderResoueceType                                            resoueceType;
    std::variant<std::shared_ptr<Image>, std::shared_ptr<Buffer>> resourceHandle;
};

class PassNode : public Node {
public:
    void Init(const std::string& passName, const std::vector<std::string>& inRourecesName,
              const std::vector<std::string>& outputResources);
    void Init(const std::vector<std::string>& inRourecesName,
              const std::vector<std::string>& outputResources);

    void CreateFrameBuffer(uint32_t width, uint32_t height); 

    void DeclareColorAttachment(const std::string& name, RenderGraphBuilder& graphBuilder, TextureDesc textureDesc, vk::ImageLayout intialLayout = vk::ImageLayout::eUndefined, vk::ImageLayout finalLayout = vk::ImageLayout::eColorAttachmentOptimal);
    void DeclareDepthAttachment(const std::string& name, RenderGraphBuilder& graphBuilder, TextureDesc textureDesc, vk::ImageLayout intialLayout = vk::ImageLayout::eUndefined, vk::ImageLayout finalLayout = vk::ImageLayout::eDepthAttachmentOptimal);

    void CreateRenderPass();

    std::string     passName;
    vk::RenderPass  renderPass;
    vk::Framebuffer frameBuffer;
    PassExecFunc    passCallback;

    std::vector<vk::AttachmentDescription> colorAttachmentDescriptions;
    vk::AttachmentDescription              depthAttachmentDescription;

    std::vector<std::shared_ptr<Image>> colorAttachments;
    std::shared_ptr<Image> depthAttachment {nullptr};

    std::shared_ptr<RenderProcess> pipelineState;
    
    std::vector<std::string> dependencyResources{};
    std::vector<std::string> outputResources{};
};

} // namespace wind