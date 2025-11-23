#pragma once

#include "renderer/vulkan/descriptors/allocator.hpp"

namespace jengine::renderer::vulkan::descriptors {

class DrawImageDescriptors {
  public:
    DrawImageDescriptors(const DescriptorAllocator& allocator, const vk::raii::Device& device, vk::ImageView draw_image_view);

    const vk::DescriptorSet& GetDescriptorSet() const { return descriptor_set; }
    vk::DescriptorSet* GetDescriptorSetPtr() { return &descriptor_set; }
    const vk::DescriptorSetLayout& GetLayout() const { return *descriptor_set_layout; }
    const vk::DescriptorSetLayout* GetLayoutPtr() { return &*descriptor_set_layout; }

  private:
    vk::DescriptorSet descriptor_set{};
    vk::raii::DescriptorSetLayout descriptor_set_layout;
    static constexpr int MAX_DESCRIPTOR_SETS = 10;
};
}  // namespace jengine::renderer::vulkan
