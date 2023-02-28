#pragma once

#include "lve_device.h"
#include "lve_pipeline.h"
#include "lve_swap_chain.h"
#include "lve_window.h"

// std
#include <memory>
#include <vector>

namespace lve {
class FirstApp {
public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp&)            = delete;
    FirstApp& operator=(const FirstApp&) = delete;

    void run();

private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    LveWindow                    m_lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    LveDevice                    m_lveDevice{m_lveWindow};
    LveSwapChain                 m_lveSwapChain{m_lveDevice, m_lveWindow.GetExtent()};
    std::unique_ptr<LvePipeline> m_lvePipeline;
    VkPipelineLayout             m_pipelineLayout;
    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace lve