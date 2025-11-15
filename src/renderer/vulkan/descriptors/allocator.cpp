#include "allocator.hpp"
#include <iostream>

namespace jengine::renderer::vulkan::descriptors {
void DescriptorAllocator::InitPool(VkDevice device,
                                   uint32_t max_sets,
                                   const std::span<PoolSizeRatio> pool_size_ratios) {
    if (initialized) {
        throw std::runtime_error("Descriptor allocator already initialized");
    }

    std::vector<VkDescriptorPoolSize> pool_sizes{};
    for (auto& ratio : pool_size_ratios) {
        pool_sizes.push_back({
            .type = ratio.type,
            .descriptorCount = static_cast<uint32_t>(max_sets * ratio.ratio),
        });
    }

    VkDescriptorPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_create_info.pPoolSizes = pool_sizes.data();
    pool_create_info.maxSets = max_sets;
    pool_create_info.flags = 0;

    if (vkCreateDescriptorPool(device, &pool_create_info, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    initialized = true;
}
void DescriptorAllocator::ClearDescriptors(VkDevice device) {
    if (!initialized) {
        std::cerr << "Warning: ClearDescriptors called but descriptor allocator is not initialized" << std::endl;
    }
    vkResetDescriptorPool(device, pool, 0);
    initialized = false;
}
void DescriptorAllocator::Destroy(VkDevice device) { vkDestroyDescriptorPool(device, pool, nullptr); }
VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet descriptor_set;
    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set");
    }
    return descriptor_set;
}
DescriptorAllocator::DescriptorAllocator(VkDevice device,
                                         uint32_t max_sets,
                                         std::initializer_list<PoolSizeRatio> pool_size_ratios,
                                         DeletionStack& deletion_stack) {
    std::vector<PoolSizeRatio> pool_size_ratios_vec(pool_size_ratios);
    InitPool(device, max_sets, pool_size_ratios_vec);
    deletion_stack.Push([this, device]() { Destroy(device); });
}
    
}
