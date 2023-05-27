#include "Descriptors.h"

#include <algorithm>

#include "Runtime/Render/Rhi/Backend.h"

namespace wind {
bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(
    const DescriptorLayoutInfo& other) const {
    if (other.bindings.size() != bindings.size()) {
        return false;
    } else {
        for (int i = 0; i < bindings.size(); ++i) {
            if (other.bindings[i].binding != bindings[i].binding) {
                return false;
            } else if (other.bindings[i].descriptorType != bindings[i].descriptorType) {
                return false;
            } else if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) {
                return false;
            } else if (other.bindings[i].stageFlags != bindings[i].stageFlags) {
                return false;
            }
        }
        return true;
    }
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const {
    using std::hash;
    using std::size_t;

    size_t result = hash<size_t>{}(bindings.size());

    for (const auto& b : bindings) {
        size_t bindHash = b.binding | (uint32_t)b.descriptorType << 8 |
                          (uint32_t)b.descriptorCount << 16 | (uint32_t)b.stageFlags << 24;
        result ^= hash<size_t>{}(bindHash);
    }

    return result;
}

void DescriptorLayoutCache::Init(vk::Device device) { m_device = device; };

vk::DescriptorSetLayout DescriptorLayoutCache::CreateDescriptorsetlayout(
    const vk::DescriptorSetLayoutCreateInfo layoutInfo) {
    DescriptorLayoutInfo ownLayoutInfo;
    uint32_t             bindCount = layoutInfo.bindingCount;
    
    ownLayoutInfo.bindings.reserve(bindCount);
    bool isSorted    = false;
    int  lastBinding = -1;

    for (uint32_t i = 0; i < bindCount; ++i) {
        ownLayoutInfo.bindings.push_back(layoutInfo.pBindings[i]);
        if (layoutInfo.pBindings[i].binding > lastBinding) {
            lastBinding = layoutInfo.pBindings[i].binding;
        } else {
            isSorted = false;
        }
    }

    if (!isSorted) {
        // sort our binding
        std::sort(ownLayoutInfo.bindings.begin(), ownLayoutInfo.bindings.end(),
                  [](const vk::DescriptorSetLayoutBinding& a,
                     const vk::DescriptorSetLayoutBinding& b) { return a.binding < b.binding; });
    }

    auto it = m_layoutCache.find(ownLayoutInfo);
    if (it != m_layoutCache.end()) {
        return it->second;
    } else {
        vk::DescriptorSetLayout retLayout = m_device.createDescriptorSetLayout(layoutInfo);
        m_layoutCache[ownLayoutInfo]      = retLayout;
        return retLayout;
    }
}

void DescriptorLayoutCache::CleanUp() {
    for (auto& [key, value] : m_layoutCache) {
        m_device.destroyDescriptorSetLayout(value);
    }
}

vk::DescriptorPool CreatePool(vk::Device device, const DescriptorAllocator::PoolSizes& poolSizes,
                              int count) {
    std::vector<vk::DescriptorPoolSize> sizes;
    sizes.reserve(poolSizes.sizes.size());

    for (const auto sz : poolSizes.sizes) {
        sizes.push_back({sz.first, uint32_t(sz.second * count)});
    }

    vk::DescriptorPoolCreateInfo createInfo;
    createInfo.setPoolSizeCount(sizes.size())
              .setPoolSizes(sizes)
              .setMaxSets(count);

    return device.createDescriptorPool(createInfo);
}

void DescriptorAllocator::Init(vk::Device device) { m_device = device; }

void DescriptorAllocator::CleanUp() {
    for (auto p : m_freePools) {
        m_device.destroyDescriptorPool(p);
    }
    for (auto p : m_usedPools) {
        m_device.destroyDescriptorPool(p);
    }
}

vk::DescriptorPool DescriptorAllocator::GrabPool() {
    if (m_freePools.size() > 0) {
        vk::DescriptorPool pool = m_freePools.back();
        m_freePools.pop_back();
        return pool;
    } else {
        return CreatePool(m_device, descriptorSizes, 1000);
    }
}

vk::DescriptorSet DescriptorAllocator::Allocate(vk::DescriptorSetLayout descriptorSetLayout) {
    if (!m_currentPool) {
        m_currentPool = GrabPool();
        m_usedPools.push_back(m_currentPool);
    }

    vk::DescriptorSetAllocateInfo allcateInfo{};
    allcateInfo.setDescriptorPool(m_currentPool)
        .setDescriptorSetCount(1)
        .setSetLayouts(descriptorSetLayout);

    vk::DescriptorSet descriptorSet;
    descriptorSet = m_device.allocateDescriptorSets(allcateInfo).front();

    return descriptorSet;
}

// DescriptorBuilder part
DescriptorBuilder DescriptorBuilder::Begin(DescriptorLayoutCache* cache,
                                           DescriptorAllocator*   allocator) {
    DescriptorBuilder builder;
    builder.m_cache.reset(cache);
    builder.m_alloc.reset(allocator);
    return builder;
}

DescriptorBuilder DescriptorBuilder::Begin() {
    DescriptorBuilder builder;
    
    builder.m_cache = RenderBackend::GetInstance().GetDescriptorLayoutCache();
    builder.m_alloc = RenderBackend::GetInstance().GetDescriptorAllocator();

    return builder;
}

DescriptorBuilder& DescriptorBuilder::BindBuffer(uint32_t                 binding,
                                                 vk::DescriptorBufferInfo bufferInfo,
                                                 vk::DescriptorType       descriptorType,
                                                 vk::ShaderStageFlags     stageFlags) {
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.setDescriptorCount(1)
        .setDescriptorType(descriptorType)
        .setBinding(binding)
        .setStageFlags(stageFlags);

    m_bindings.push_back(newBinding);

    vk::WriteDescriptorSet newWrite;
    newWrite.setDescriptorCount(1)
        .setDescriptorType(descriptorType)
        .setDstBinding(binding)
        .setBufferInfo(bufferInfo);

    m_writes.push_back(newWrite);
    return *this;
}

DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t binding, vk::DescriptorImageInfo imageInfo,
                                                vk::DescriptorType   descriptorType,
                                                vk::ShaderStageFlags stageFlags) {
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.setDescriptorCount(1)
        .setDescriptorType(descriptorType)
        .setBinding(binding)
        .setStageFlags(stageFlags);

    m_bindings.push_back(newBinding);

    vk::WriteDescriptorSet newWrite;
    newWrite.setDescriptorCount(1)
        .setDescriptorType(descriptorType)
        .setDstBinding(binding)
        .setImageInfo(imageInfo);

    m_writes.push_back(newWrite);
    return *this;
}

bool DescriptorBuilder::Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout) {
    const auto&                       device = RenderBackend::GetInstance().GetDevice();
    vk::DescriptorSetLayoutCreateInfo createInfo;
    createInfo.setBindingCount(m_bindings.size()).setBindings(m_bindings);

    layout = m_cache->CreateDescriptorsetlayout(createInfo);

    set = m_alloc->Allocate(layout);

    for (auto& w : m_writes) {
        w.setDstSet(set);
    }

    device.updateDescriptorSets(m_writes.size(), m_writes.data(), 0, nullptr);
    
    return true;
}

bool DescriptorBuilder::Build(vk::DescriptorSet& set) {
    const auto&                       device = RenderBackend::GetInstance().GetDevice();
    vk::DescriptorSetLayoutCreateInfo createInfo;
    createInfo.setBindingCount(m_bindings.size()).setBindings(m_bindings);

    auto layout = m_cache->CreateDescriptorsetlayout(createInfo);

    set = m_alloc->Allocate(layout);

    for (auto& w : m_writes) {
        w.setDstSet(set);
    }

    device.updateDescriptorSets(m_writes.size(), m_writes.data(), 0, nullptr);
    
    return true;
}
} // namespace wind