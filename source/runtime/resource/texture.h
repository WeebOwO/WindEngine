#pragma once

#include <vulkan/vulkan.hpp>
#include <string_view>

namespace wind {
class Texture {
public:
    Texture(std::string_view filepath);
    ~Texture();

    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView view;
};

}