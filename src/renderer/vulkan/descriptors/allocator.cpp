#include "allocator.hpp"

#include <vector>

#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::descriptors {
DescriptorAllocator::DescriptorAllocator(const vk::raii::Device& device,
                                         uint32_t max_sets,
                                         const vk::ArrayProxy<PoolSizeRatio> pool_size_ratios)
    : pool(nullptr), device(device) {
    std::vector<vk::DescriptorPoolSize> pool_sizes{};
    for (auto& ratio : pool_size_ratios) {
        pool_sizes.push_back({
            .type = ratio.type,
            .descriptorCount = static_cast<uint32_t>(max_sets * ratio.ratio),
        });
    }
    pool = device.createDescriptorPool({
        .maxSets = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    });
}
vk::DescriptorSet DescriptorAllocator::Allocate(const vk::DescriptorSetLayout& layout) const {
    auto sets = device.allocateDescriptorSets({
        .descriptorPool = pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    });
    return sets[0];
}
void DescriptorAllocator::ClearDescriptors() const { pool.reset(); }
}  // namespace jengine::renderer::vulkan::descriptors
