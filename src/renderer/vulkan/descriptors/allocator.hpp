#pragma once

#include <vulkan/vulkan_core.h>
#include <initializer_list>
#include "renderer/vulkan/deletion_stack.hpp"
namespace jengine::renderer::vulkan::descriptors {
class DescriptorAllocator {
  public:
    struct PoolSizeRatio {
        VkDescriptorType type{};
        float ratio{};
    };


    DescriptorAllocator(VkDevice device,
                        uint32_t max_sets,
                        std::initializer_list<PoolSizeRatio> pool_size_ratios,
                        DeletionStack& deletion_stack);

    void InitPool(VkDevice device, uint32_t max_sets, const std::span<PoolSizeRatio> pool_size_ratios);
    void ClearDescriptors(VkDevice device);
    void Destroy(VkDevice device);

    VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);

  private:
    bool initialized = false;
    VkDescriptorPool pool{};
};
}
