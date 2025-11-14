#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <optional>
namespace jengine::renderer::vulkan::init {

VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t graphics_queue_family_index, VkCommandPoolCreateFlags flags);
VkCommandBufferAllocateInfo PrimaryCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags);
VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags);
VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags usage_flags);
VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect_mask);
VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags stage_flags);
VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer command_buffer);
VkSubmitInfo2 SubmitInfo2(VkCommandBufferSubmitInfo* command_buffer_submit_info,
                              VkSemaphoreSubmitInfo* wait_semaphore_submit_info,
                              VkSemaphoreSubmitInfo* signal_semaphore_submit_info);
VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);
VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
// if clear_value is set, image will be cleared with the color
VkRenderingAttachmentInfo RenderingAttachmentInfo(VkImageView image_view,
                                                  std::optional<VkClearValue> clear_value,
                                                  VkImageLayout image_layout);

}  // namespace jengine::renderer::vulkan
