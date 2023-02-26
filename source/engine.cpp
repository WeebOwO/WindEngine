#include "engine.h"
#include "runtime/render/vulkan/vulkan_rhi.h"
#include <memory>

void Engine::Run() {
    while (!m_window.ShouldClose()) {
        glfwPollEvents();
    }
}

Engine::Engine() { 
    const std::string vertexFilepath = "simple_shader.frag.spv";
    const std::string fragFilepath = "simple_shader.vert.spv";
    m_rhi = std::make_shared<VulkanRHI>();
}
