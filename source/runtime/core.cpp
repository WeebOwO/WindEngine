#include "core.h"
#include "runtime/core.h"

#include <chrono>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "runtime/render/renderer.h"

constexpr int WIDTH  = 1920;
constexpr int HEIGHT = 1080;

namespace wind {
class EngineImpl {
public:
    EngineImpl() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);  
        m_renderer = std::make_unique<Renderer>(m_window);
    }

    ~EngineImpl() { glfwDestroyWindow(m_window); }

    void Run() {
        auto               startTime = std::chrono::high_resolution_clock::now();
        auto               previous  = std::chrono::high_resolution_clock::now();
        decltype(previous) current;
        float              delta;

        while (!glfwWindowShouldClose(m_window)) {    
            LogicTick();
            RenderTick();
        }
    }

private:
    GLFWwindow* m_window;
    std::unique_ptr<Renderer> m_renderer;
    void RenderTick() {}
    void LogicTick() { glfwPollEvents(); }
};

void Engine::Run() { m_impl->Run(); }

Engine::Engine() : m_impl(std::make_unique<EngineImpl>()) {}
Engine::~Engine() = default;
} // namespace wind