#include "context.h"

#include <iostream>
#include <vector>


#include "misc.h"
#include "runtime/context.h"

namespace wind {
std::unique_ptr<RenderContext> RenderContext::m_instance = nullptr;

RenderContext::RenderContext() noexcept {
    CreateInstance();
    PickupPhysicalDevice();
    QueryQueueFamilyIndices();
    CreateDevice();
    GetQueue();
}

void RenderContext::CreateInstance() {
    vk::InstanceCreateInfo createinfo {};
    vk::ApplicationInfo    appInfo{};

    appInfo.setApiVersion(VK_MAKE_VERSION(1, 0, 3));
    createinfo.setPApplicationInfo(&appInfo);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
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

void RenderContext::Init() { m_instance.reset(new RenderContext); }

void RenderContext::Quit() { m_instance.reset(); }

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