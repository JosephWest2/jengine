#include "descriptors.hpp"

#include <vulkan/vulkan_core.h>

#include <iostream>
#include <stdexcept>

namespace jengine::renderer::vulkan {

void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorCount = 1;
    layout_binding.descriptorType = type;
    bindings.push_back(layout_binding);
}
void DescriptorLayoutBuilder::Clear() { bindings.clear(); }
VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device,
                                                     VkShaderStageFlags shader_stage_flags,
                                                     void* pnext,
                                                     VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shader_stage_flags;
    }

    VkDescriptorSetLayoutCreateInfo layout_create_info{};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_create_info.pBindings = bindings.data();
    layout_create_info.pNext = pnext;
    layout_create_info.flags = flags;

    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(device, &layout_create_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
    return layout;
}
void DescriptorAllocator::InitPool(VkDevice device,
                                   uint32_t max_sets,
                                   const std::span<PoolSizeRatio> pool_size_ratios) {
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
}
void DescriptorAllocator::ClearDescriptors(VkDevice device) { vkResetDescriptorPool(device, pool, 0); }
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
}  // namespace jengine::renderer::vulkan
