#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

#include "runtime/base/macro.h"
#include "runtime/render/vulkan/vulkan_rhi.h"
#include "runtime/render/window.h"

static constexpr uint32_t width = 800;
static constexpr uint32_t height = 600;

class Engine  {
public:
    Engine();
    ~Engine() = default;
    DELETE_MOVE(Engine);
    DELETE_COPY(Engine);
    void Run();
private:
    Window m_window {width, height, "Vulkan Game Engine"};
    std::shared_ptr<VulkanRHI> m_rhi;
};