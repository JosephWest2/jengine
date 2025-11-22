#include "manager.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/descriptors/allocator.hpp"

namespace jengine::renderer::vulkan::descriptors {

Manager::Manager(const vk::Device& device, const vk::ImageView& draw_image_view)
    : global_allocator(device, 10, {{vk::DescriptorType::eStorageImage, 1.0f}}),
      draw_image_descriptors(global_allocator, device, draw_image_view) {}
}  // namespace jengine::renderer::vulkan::descriptors
