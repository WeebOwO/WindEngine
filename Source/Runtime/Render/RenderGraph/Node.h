#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Render/RHI/Image.h"

#include "Runtime/Render/RenderGraph/Node.h"
#include "Runtime/Render/RenderGraph/RenderPass.h"
#include "Runtime/Render/RenderGraph/RenderResource.h"

namespace wind {
class RenderGraph;
class RenderGraphRegister;
class CommandBuffer;
class PassNode;
class RenderGraphBuilder;
class RenderProcess;
class SceneView;

using PassExecFunc  = std::function<void(CommandBuffer&, RenderGraphRegister*)>;
using PassSetupFunc = std::function<PassExecFunc(PassNode*)>;

enum class RenderResoueceType : uint8_t { Buffer = 0, Image };

class Node {
protected:
    uint32_t inRefCnt  = 0;
    uint32_t outRefcnt = 0;
    bool     nevelCull = false;
};

class ResourceNode : public Node {
public:
    std::string             resourceName;
    RenderResoueceType      resoueceType;
    std::shared_ptr<Image>  imageHandle;
    std::shared_ptr<Buffer> bufferHandle;
};

class PassNode : public Node {
public:
    ~PassNode();
    void Init(const std::string& passName, const std::vector<std::string>& inRourecesName,
              const std::vector<std::string>& outputResources);
    void Init(const std::vector<std::string>& inRourecesName,
              const std::vector<std::string>& outputResources);

    void CreateFrameBuffer(uint32_t width, uint32_t height);
    void SetRenderRect(uint32_t width, uint32_t height) {
        renderRect.width = width, renderRect.height = height;
    }
    void
    DeclareColorAttachment(const std::string& name, const TextureDesc& textureDesc,
                           vk::ImageLayout intialLayout = vk::ImageLayout::eUndefined,
                           vk::ImageLayout finalLayout  = vk::ImageLayout::eColorAttachmentOptimal);
    void
    DeclareDepthAttachment(const std::string& name, const TextureDesc& textureDesc,
                           vk::ImageLayout intialLayout = vk::ImageLayout::eUndefined,
                           vk::ImageLayout finalLayout  = vk::ImageLayout::eDepthAttachmentOptimal);

    void ConstructResource(RenderGraphBuilder& graphBuilder);

    void CreateRenderPass();

    std::string     passName;
    vk::RenderPass  renderPass;
    vk::Framebuffer frameBuffer;
    PassExecFunc    passCallback;

    std::pmr::vector<vk::AttachmentDescription> colorAttachmentDescriptions;
    vk::AttachmentDescription                   depthAttachmentDescription;

    std::pmr::vector<std::shared_ptr<Image>> colorAttachments;
    std::shared_ptr<Image>                   depthAttachment{nullptr};

    std::vector<vk::ClearValue> colorClearValue;
    vk::ClearValue              depthClearValue;

    std::shared_ptr<RenderProcess> pipelineState;

    std::pmr::vector<std::string> dependencyResources{};
    std::pmr::vector<std::string> outputResources{};

    std::pmr::unordered_map<std::string, TextureDesc> colorTextureDescs;
    std::pmr::unordered_map<std::string, TextureDesc> depthTextureDesc;
    // set for render pass
    struct RenderRect {
        uint32_t width, height;
    } renderRect;

    SceneView* renderScene = nullptr;
};

} // namespace wind