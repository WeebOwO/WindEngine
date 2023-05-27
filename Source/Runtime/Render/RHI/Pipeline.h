#pragma once

#include <vulkan/vulkan.hpp>

#include "Runtime/Render/RHI/Shader.h"

namespace wind {
vk::Pipeline CreateGraphicsPipeline(const GraphicsShader& shader, vk::RenderPass renderpass, vk::PipelineLayout pipelinelayout);
} // namespace wind