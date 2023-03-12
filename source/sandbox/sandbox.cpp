#include "GLFW/glfw3.h"
#include "runtime/core.h"
#include "runtime/window.h"

int main() {
    wind::Window window(800, 600, "Vulkan Graphic Engine");
    wind::Init();

    while(!window.ShouldClose()) {
        glfwPollEvents();
    }

    wind::Quit();
    
    return 0;
}