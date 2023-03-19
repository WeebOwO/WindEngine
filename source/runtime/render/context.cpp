#include "context.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "GLFW/glfw3.h"
#include "runtime/base/misc.h"

static std::vector<const char*> extensions = {};
static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

namespace wind {

void RenderContext::CreateInstance() {
    vk::InstanceCreateInfo createinfo {};
    vk::ApplicationInfo    appInfo{};

    appInfo.setApiVersion(VK_MAKE_VERSION(1, 0, 3));
    createinfo.setPApplicationInfo(&appInfo);

    RemoveNosupportedElems<const char*, vk::LayerProperties>(
        layers, vk::enumerateInstanceLayerProperties(),
        [](const char* e1, const vk::LayerProperties& e2) {
            return std::strcmp(e1, e2.layerName) == 0;
        });

    createinfo.setPEnabledLayerNames(layers);

    vkInstance = vk::createInstance(createinfo);
}

RenderContext::~RenderContext() {
    device.destroy();
    vkInstance.destroy();
}

void RenderContext::PickupPhysicalDevice() {
    auto physicalDevices = vkInstance.enumeratePhysicalDevices();
    // not good, but enough for me
    physicalDevice = physicalDevices.front();
    std::cout << physicalDevice.getProperties().deviceName << "\n";
}

void RenderContext::CreateDevice() {
    vk::DeviceCreateInfo      createInfo;
    vk::DeviceQueueCreateInfo queueCreateInfo;

    float priority = 1.0;
    queueCreateInfo.setPQueuePriorities(&priority)
                   .setQueueCount(1)
                   .setQueueFamilyIndex(queueIndices.graphicsQueueIndex.value());

    createInfo.setQueueCreateInfos(queueCreateInfo);
              
    device = physicalDevice.createDevice(createInfo);
}

void RenderContext::QueryQueueFamilyIndices() {
    auto properties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; const auto& queueFamily : properties) {
        if (queueFamily.queueFlags | vk::QueueFlagBits::eGraphics) {
            queueIndices.graphicsQueueIndex = i;
        }
        if (queueIndices.IsComplete()) { break; }
        ++i;
    }
}

void RenderContext::GetQueue() {
    graphicsQueue = device.getQueue(queueIndices.graphicsQueueIndex.value(), 0);   
}    

} // namespace wind