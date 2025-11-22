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

    DescriptorAllocator(const vk::Device& device,
                        uint32_t max_sets,
                        const vk::ArrayProxy<PoolSizeRatio> pool_size_ratios);
    ~DescriptorAllocator();

    void ClearDescriptors() const;

    vk::DescriptorSet Allocate(const vk::DescriptorSetLayout& layout) const;

  private:
    vk::DescriptorPool pool;

    const vk::Device& device;
};
}  // namespace jengine::renderer::vulkan::descriptors
