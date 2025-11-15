#pragma once

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/descriptors/allocator.hpp"
#include "renderer/vulkan/descriptors/draw_image_descriptors.hpp"

namespace jengine::renderer::vulkan::descriptors {
class Manager {
  public:
    Manager(VkDevice device, VkImageView draw_image_view, DeletionStack& deletion_stack);
    VkDescriptorSet* GetDrawImageDescriptorSetPtr() { return draw_image_descriptors.GetDescriptorSetPtr(); }
    VkDescriptorSetLayout* GetDrawImageDescriptorLayoutPtr() { return draw_image_descriptors.GetLayoutPtr(); }

  private:
    DescriptorAllocator global_allocator;
    DrawImageDescriptors draw_image_descriptors;
};
}  // namespace jengine::renderer::vulkan::descriptors
