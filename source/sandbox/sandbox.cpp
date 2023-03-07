#include "GLFW/glfw3.h"
#include "runtime/window.h"

int main() {
    wind::Window window(800, 600, "Vulkan Graphic Engine");
    while(!window.ShouldClose()) {
        // to do
        glfwPollEvents();
    }
    return 0;
}