#pragma once

#include <vulkan/vulkan.hpp>

struct VkSampler_T;
using VkSampler = VkSampler_T*;

namespace wind {
class Sampler {
public:
    enum class Filter : uint8_t { NEAREST = 0, LINEAR };

    using MinFilter = Filter;
    using MagFilter = Filter;
    using MipFilter = Filter;

    enum class AddressMode : uint8_t {
        REPEAT = 0,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    };

    Sampler() = default;
    ~Sampler();
    Sampler(Sampler&& other) noexcept;
    Sampler& operator=(Sampler&& other) noexcept;
    Sampler(MinFilter minFilter, MagFilter magFilter, AddressMode uvwAddress, MipFilter mipFilter);
    void Init(MinFilter minFilter, MagFilter magFilter, AddressMode uvwAddress,
              MipFilter mipFilter);

    [[nodiscard]] const vk::Sampler& GetNativeHandle() const;

private:
    vk::Sampler m_handle;
    void        Destroy();
};

using SamplerReference = std::reference_wrapper<const Sampler>;
} // namespace wind