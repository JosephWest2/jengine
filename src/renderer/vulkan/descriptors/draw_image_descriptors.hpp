#pragma once

#include "renderer/vulkan/deletion_stack.hpp"
#include "renderer/vulkan/descriptors/allocator.hpp"

namespace jengine::renderer::vulkan::descriptors {

class DrawImageDescriptors {
  public:
    DrawImageDescriptors(DescriptorAllocator& allocator, VkDevice device, VkImageView draw_image_view, DeletionStack& deletion_stack);

    VkDescriptorSet GetDescriptorSet() const { return descriptor_set; }
    VkDescriptorSet* GetDescriptorSetPtr() { return &descriptor_set; }
    VkDescriptorSetLayout GetLayout() const { return descriptor_set_layout; }
    VkDescriptorSetLayout* GetLayoutPtr() { return &descriptor_set_layout; }

    void Destroy(VkDevice device);

  private:
    VkDescriptorSet descriptor_set{};
    VkDescriptorSetLayout descriptor_set_layout{};
    static constexpr int MAX_DESCRIPTOR_SETS = 10;
};
}  // namespace jengine::renderer::vulkan
