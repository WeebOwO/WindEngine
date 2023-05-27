#include "Sampler.h"

#include "Runtime/Render/RHI/Backend.h"

namespace wind {
const vk::Sampler& Sampler::GetNativeHandle() const { return m_handle; }

vk::Filter FilterToNative(Sampler::Filter filter) {
    switch (filter) {
    case Sampler::Filter::NEAREST:
        return vk::Filter::eNearest;
    case Sampler::Filter::LINEAR:
        return vk::Filter::eLinear;
    default:
        assert(false);
        return vk::Filter::eNearest;
    }
}

vk::SamplerMipmapMode MipmapToNative(Sampler::Filter filter) {
    switch (filter) {
    case Sampler::Filter::NEAREST:
        return vk::SamplerMipmapMode::eNearest;
    case Sampler::Filter::LINEAR:
        return vk::SamplerMipmapMode::eLinear;
    default:
        assert(false);
        return vk::SamplerMipmapMode::eNearest;
    }
}

vk::SamplerAddressMode AddressToNative(Sampler::AddressMode address) {
    switch (address) {
    case Sampler::AddressMode::REPEAT:
        return vk::SamplerAddressMode::eRepeat;
    case Sampler::AddressMode::MIRRORED_REPEAT:
        return vk::SamplerAddressMode::eMirroredRepeat;
    case Sampler::AddressMode::CLAMP_TO_EDGE:
        return vk::SamplerAddressMode::eClampToEdge;
    case Sampler::AddressMode::CLAMP_TO_BORDER:
        return vk::SamplerAddressMode::eClampToBorder;
    default:
        assert(false);
        return vk::SamplerAddressMode::eClampToEdge;
    }
}

Sampler::Sampler(MinFilter minFilter, MagFilter magFilter, AddressMode uvwAddress,
                 MipFilter mipFilter) {
    Init(minFilter, magFilter, uvwAddress, mipFilter);
}

void Sampler::Init(MinFilter minFilter, MagFilter magFilter, AddressMode uvwAddress,
                   MipFilter mipFilter) {
    vk::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.setMinFilter(FilterToNative(minFilter))
        .setMagFilter(FilterToNative(magFilter))
        .setAddressModeU(AddressToNative(uvwAddress))
        .setAddressModeV(AddressToNative(uvwAddress))
        .setAddressModeW(AddressToNative(uvwAddress))
        .setMipmapMode(MipmapToNative(mipFilter))
        .setMipLodBias(0)
        .setMinLod(0)
        .setMaxLod(1000);

    m_handle = RenderBackend::GetInstance().GetDevice().createSampler(samplerCreateInfo);
}

void Sampler::Destroy() {
    if ((bool)m_handle) RenderBackend::GetInstance().GetDevice().destroySampler(m_handle);
}

Sampler::~Sampler() { Destroy(); }

Sampler::Sampler(Sampler&& other) noexcept {
    m_handle       = other.m_handle;
    other.m_handle = vk::Sampler{};
}

Sampler& Sampler::operator=(Sampler&& other) noexcept {
    Destroy();
    m_handle       = other.m_handle;
    other.m_handle = vk::Sampler{};
    return *this;
}
} // namespace wind