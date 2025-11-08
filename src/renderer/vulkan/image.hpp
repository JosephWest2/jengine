#pragma once

#include <vulkan/vulkan_core.h>
namespace jengine::renderer::vulkan {

void TransitionImage(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout);

}  // namespace jengine::renderer::vulkan
