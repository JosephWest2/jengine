#pragma once

#include <vulkan/vulkan_core.h>

#include <initializer_list>
#include <span>

namespace jengine::renderer::vulkan::descriptors {
class DescriptorAllocator {
  public:
    struct PoolSizeRatio {
        VkDescriptorType type{};
        float ratio{};
    };

    DescriptorAllocator(VkDevice& device, uint32_t max_sets, std::initializer_list<PoolSizeRatio> pool_size_ratios);
    ~DescriptorAllocator();

    void InitPool(uint32_t max_sets, const std::span<PoolSizeRatio> pool_size_ratios);
    void ClearDescriptors();

    VkDescriptorSet Allocate(VkDescriptorSetLayout layout);

  private:
    bool initialized = false;
    VkDescriptorPool pool{};

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan::descriptors
