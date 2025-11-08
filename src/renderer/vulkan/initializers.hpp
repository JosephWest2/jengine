#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
namespace jengine::renderer::vulkan {

VkCommandPoolCreateInfo InitCommandPoolCreateInfo(uint32_t graphics_queue_family_index, VkCommandPoolCreateFlags flags);
VkCommandBufferAllocateInfo InitPrimaryCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
VkFenceCreateInfo InitFenceCreateInfo(VkFenceCreateFlags flags);
VkSemaphoreCreateInfo InitSemaphoreCreateInfo(VkSemaphoreCreateFlags flags);
VkCommandBufferBeginInfo InitCommandBufferBeginInfo(VkCommandBufferUsageFlags usage_flags);
VkImageSubresourceRange InitImageSubresourceRange(VkImageAspectFlags aspect_mask);
VkSemaphoreSubmitInfo InitSemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags stage_flags);
VkCommandBufferSubmitInfo InitCommandBufferSubmitInfo(VkCommandBuffer command_buffer);
VkSubmitInfo2 InitSubmitInfo2(VkCommandBufferSubmitInfo* command_buffer_submit_info,
                              VkSemaphoreSubmitInfo* wait_semaphore_submit_info,
                              VkSemaphoreSubmitInfo* signal_semaphore_submit_info);

}  // namespace jengine::renderer::vulkan
