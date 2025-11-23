#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace jengine::renderer::vulkan::descriptors {
class DescriptorAllocator {
  public:
    struct PoolSizeRatio {
        vk::DescriptorType type{};
        float ratio{};
    };

    DescriptorAllocator(const vk::raii::Device& device,
                        uint32_t max_sets,
                        const vk::ArrayProxy<PoolSizeRatio> pool_size_ratios);

    void ClearDescriptors() const;

    vk::DescriptorSet Allocate(const vk::Device& device, const vk::DescriptorSetLayout& layout) const;

  private:
    vk::raii::DescriptorPool pool;
};
}  // namespace jengine::renderer::vulkan::descriptors
