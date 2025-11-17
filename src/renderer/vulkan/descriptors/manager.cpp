#include "manager.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/descriptors/allocator.hpp"

namespace jengine::renderer::vulkan::descriptors {

Manager::Manager(VkDevice& device, VkImageView draw_image_view)
    : global_allocator(device, 10, {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f}}),
      draw_image_descriptors(global_allocator, device, draw_image_view) {}
}  // namespace jengine::renderer::vulkan::descriptors
