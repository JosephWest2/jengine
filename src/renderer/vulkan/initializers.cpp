#include "initializers.hpp"
#include <vulkan/vulkan_core.h>
namespace jengine::renderer::vulkan {

VkCommandPoolCreateInfo InitCommandPoolCreateInfo(uint32_t graphics_queue_family_index,
                                                                               VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.queueFamilyIndex = graphics_queue_family_index;
    create_info.flags = flags;
    return create_info;
}

VkCommandBufferAllocateInfo InitPrimaryCommandBufferAllocateInfo(
    VkCommandPool pool,
    uint32_t count) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;
    return alloc_info;
}

VkFenceCreateInfo InitFenceCreateInfo(VkFenceCreateFlags flags) {
    VkFenceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = flags;
    return create_info;
}

VkSemaphoreCreateInfo InitSemaphoreCreateInfo(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = flags;
    return create_info;
}
VkCommandBufferBeginInfo InitCommandBufferBeginInfo(VkCommandBufferUsageFlags usage_flags) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.pInheritanceInfo = nullptr;
    begin_info.flags = usage_flags;
    return begin_info;
}
VkImageSubresourceRange InitImageSubresourceRange(VkImageAspectFlags aspect_mask) {
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = aspect_mask;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = VK_REMAINING_MIP_LEVELS;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return subresource_range;

}
VkSemaphoreSubmitInfo InitSemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags stage_flags) {
    VkSemaphoreSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.semaphore = semaphore;
    submit_info.stageMask = stage_flags;
    submit_info.deviceIndex = 0;
    submit_info.value = 0;
    return submit_info;
}
VkCommandBufferSubmitInfo InitCommandBufferSubmitInfo(VkCommandBuffer command_buffer) {
    VkCommandBufferSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.commandBuffer = command_buffer;
    submit_info.deviceMask = 0;
    return submit_info;
}
VkSubmitInfo2 InitSubmitInfo2(VkCommandBufferSubmitInfo* command_buffer_submit_info,
                              VkSemaphoreSubmitInfo* wait_semaphore_submit_info,
                              VkSemaphoreSubmitInfo* signal_semaphore_submit_info) {
    VkSubmitInfo2 submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext = nullptr;

    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = command_buffer_submit_info;

    submit_info.signalSemaphoreInfoCount = signal_semaphore_submit_info ? 1 : 0;
    submit_info.pSignalSemaphoreInfos = signal_semaphore_submit_info;

    submit_info.waitSemaphoreInfoCount = wait_semaphore_submit_info ? 1 : 0;
    submit_info.pWaitSemaphoreInfos = wait_semaphore_submit_info;

    return submit_info;
}
}  // namespace jengine::renderer::vulkan
