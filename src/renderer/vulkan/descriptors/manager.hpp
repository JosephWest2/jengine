#pragma once

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/descriptors/allocator.hpp"
#include "renderer/vulkan/descriptors/draw_image_descriptors.hpp"

namespace jengine::renderer::vulkan::descriptors {
class Manager {
  public:
    Manager(const vk::Device& device, const vk::ImageView& draw_image_view);
    vk::DescriptorSet* GetDrawImageDescriptorSetPtr() { return draw_image_descriptors.GetDescriptorSetPtr(); }
    vk::DescriptorSetLayout* GetDrawImageDescriptorLayoutPtr() { return draw_image_descriptors.GetLayoutPtr(); }

  private:
    DescriptorAllocator global_allocator;
    DrawImageDescriptors draw_image_descriptors;
};
}  // namespace jengine::renderer::vulkan::descriptors
