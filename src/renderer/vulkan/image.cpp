#include "image.hpp"
#include <vulkan/vulkan_core.h>
#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan {
void TransitionImage(VkCommandBuffer command_buffer,
                     VkImage image,
                     VkImageLayout old_layout,
                     VkImageLayout new_layout) {

    VkImageMemoryBarrier2 image_barrier{};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_barrier.pNext = nullptr;

    image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    image_barrier.oldLayout = old_layout;
    image_barrier.newLayout = new_layout;

    image_barrier.image = image;

    VkImageAspectFlags aspect_mask = old_layout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange = InitImageSubresourceRange(aspect_mask);

    VkDependencyInfo dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.pNext = nullptr;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_barrier;

    vkCmdPipelineBarrier2(command_buffer, &dependency_info);
}
}  // namespace jengine::renderer::vulkan
