#pragma once

#include <vulkan/vulkan_core.h>

#include <span>
#include <vector>

namespace jengine::renderer::vulkan {

class DescriptorLayoutBuilder {
  public:
    void AddBinding(uint32_t binding, VkDescriptorType type);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice device,
                                VkShaderStageFlags shader_stage_flags,
                                void* pnext = nullptr,
                                VkDescriptorSetLayoutCreateFlags flags = 0);

  private:
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
};

class DescriptorAllocator {
  public:
    struct PoolSizeRatio {
        VkDescriptorType type{};
        float ratio{};
    };

    void InitPool(VkDevice device, uint32_t max_sets, const std::span<PoolSizeRatio> pool_size_ratios);
    void ClearDescriptors(VkDevice device);
    void Destroy(VkDevice device);

    VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);

  private:
    VkDescriptorPool pool{};
};

}  // namespace jengine::renderer::vulkan
