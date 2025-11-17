#pragma once

#include "renderer/vulkan/descriptors/allocator.hpp"

namespace jengine::renderer::vulkan::descriptors {

class DrawImageDescriptors {
  public:
    DrawImageDescriptors(DescriptorAllocator& allocator, VkDevice& device, VkImageView draw_image_view);
    ~DrawImageDescriptors();

    VkDescriptorSet GetDescriptorSet() const { return descriptor_set; }
    VkDescriptorSet* GetDescriptorSetPtr() { return &descriptor_set; }
    VkDescriptorSetLayout GetLayout() const { return descriptor_set_layout; }
    VkDescriptorSetLayout* GetLayoutPtr() { return &descriptor_set_layout; }


  private:
    VkDescriptorSet descriptor_set{};
    VkDescriptorSetLayout descriptor_set_layout{};
    static constexpr int MAX_DESCRIPTOR_SETS = 10;

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan
