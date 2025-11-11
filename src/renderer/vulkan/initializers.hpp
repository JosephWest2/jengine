#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
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

}  // namespace jengine::renderer::vulkan
