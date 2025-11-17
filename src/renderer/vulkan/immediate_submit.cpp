#include "immediate_submit.hpp"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan {
ImmediateSubmit::ImmediateSubmit(VkDevice& device, uint32_t graphics_queue_family_index) : device(device) {
    VkCommandPoolCreateInfo command_pool_create_info =
        init::CommandPoolCreateInfo(graphics_queue_family_index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create immediate submit command pool");
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info = init::PrimaryCommandBufferAllocateInfo(command_pool, 1);
    if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate immediate submit command buffer");
    }

    VkFenceCreateInfo fence_create_info = init::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    if (vkCreateFence(device, &fence_create_info, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create immediate submit fence");
    }
};
void ImmediateSubmit::Submit(VkQueue queue, std::function<void(VkCommandBuffer)> function) {
    if (vkResetFences(device, 1, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset immediate submit fence");
    }
    if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset immediate submit command buffer");
    }

    VkCommandBufferBeginInfo command_buffer_begin_info =
        init::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin immediate submit command buffer");
    }

    function(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end immediate submit command buffer");
    }

    VkCommandBufferSubmitInfo command_buffer_submit_info = init::CommandBufferSubmitInfo(command_buffer);
    VkSubmitInfo2 submit_info_2 = init::SubmitInfo2(&command_buffer_submit_info, nullptr, nullptr);

    if (vkQueueSubmit2(queue, 1, &submit_info_2, fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit immediate submit command buffer");
    }

    if (vkWaitForFences(device, 1, &fence, true, 1000000000) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for immediate submit fence");
    }
}
ImmediateSubmit::~ImmediateSubmit() {
    vkDestroyCommandPool(device, command_pool, nullptr);
    vkDestroyFence(device, fence, nullptr);
}
}  // namespace jengine::renderer::vulkan
