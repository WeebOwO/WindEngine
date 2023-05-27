#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.hpp"

namespace wind {

class DescriptorLayoutCache {
public:
    struct DescriptorLayoutInfo {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bool                 operator==(const DescriptorLayoutInfo& other) const;
        [[nodiscard]] size_t hash() const;
    };
    vk::DescriptorSetLayout CreateDescriptorsetlayout(const vk::DescriptorSetLayoutCreateInfo layoutInfo);

    void Init(vk::Device);
    void CleanUp();

private:
    struct DescriptorLayoutHash {
        std::size_t operator()(const DescriptorLayoutInfo& k) const { return k.hash(); }
    };
    using LayoutCache =
        std::unordered_map<DescriptorLayoutInfo, vk::DescriptorSetLayout, DescriptorLayoutHash>;
        
    vk::Device m_device;
    LayoutCache m_layoutCache;
};

class DescriptorAllocator {
public:
    struct PoolSizes {
        std::vector<std::pair<vk::DescriptorType, float>> sizes = {
            {vk::DescriptorType::eSampler, 0.5f},
            {vk::DescriptorType::eCombinedImageSampler, 4.f},
            {vk::DescriptorType::eUniformBuffer, 2.f},
            {vk::DescriptorType::eStorageBuffer, 2.f},
            {vk::DescriptorType::eInputAttachment, 0.5f},
            {vk::DescriptorType::eUniformBufferDynamic, 0.5f},
            {vk::DescriptorType::eStorageBufferDynamic, 0.5f}};
    };

    void Init(vk::Device);
    void CleanUp();

    vk::DescriptorSet Allocate(vk::DescriptorSetLayout);

private:
    vk::Device         m_device;
    vk::DescriptorPool GrabPool();

    PoolSizes descriptorSizes;

    vk::DescriptorPool              m_currentPool{nullptr};
    std::vector<vk::DescriptorPool> m_usedPools;
    std::vector<vk::DescriptorPool> m_freePools;
};

class DescriptorBuilder {
public:
    static DescriptorBuilder Begin(DescriptorLayoutCache* cache, DescriptorAllocator* allocator);
    static DescriptorBuilder Begin();
    DescriptorBuilder& BindBuffer(uint32_t binding, vk::DescriptorBufferInfo bufferInfo, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags);
    DescriptorBuilder& BindImage(uint32_t binding, vk::DescriptorImageInfo imageInfo, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags);
    
    bool Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
    bool Build(vk::DescriptorSet& set);
    
private:
    std::vector<vk::WriteDescriptorSet> m_writes;
    std::vector<vk::DescriptorSetLayoutBinding> m_bindings;

    std::shared_ptr<DescriptorLayoutCache> m_cache;
    std::shared_ptr<DescriptorAllocator> m_alloc;
};
} // namespace wind