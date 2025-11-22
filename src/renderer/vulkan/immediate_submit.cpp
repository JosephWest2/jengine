#include "immediate_submit.hpp"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "renderer/vulkan/initializers.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {
ImmediateSubmit::ImmediateSubmit(const vk::raii::Device& device, uint32_t graphics_queue_family_index)
    : command_pool(device.createCommandPool({.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                             .queueFamilyIndex = graphics_queue_family_index})),
      fence(device.createFence({.flags = vk::FenceCreateFlagBits::eSignaled})) {
    auto buffers = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{
        .commandPool = command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    });
    command_buffer = std::move(buffers.front());
}
void ImmediateSubmit::Submit(const vk::Device& device,
                             const vk::Queue& queue,
                             std::function<void(vk::CommandBuffer)>&& function) {
    if (device.waitForFences(1, &(*fence), true, TIMEOUT_ONE_SECOND) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for immediate submit fence");
    }
    if (device.resetFences(1, &(*fence)) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to reset immediate submit fence");
    }

    command_buffer.reset();

    command_buffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    function(command_buffer);

    command_buffer.end();

    vk::CommandBufferSubmitInfo command_buffer_submit_info{
        .commandBuffer = command_buffer,
    };
    vk::SubmitInfo2 submit_info_2 = init::SubmitInfo2(&command_buffer_submit_info, nullptr, nullptr);

    if (queue.submit2(1, &submit_info_2, fence) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to submit immediate submit command buffer");
    }

    if (device.waitForFences(1, &(*fence), true, TIMEOUT_ONE_SECOND) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for immediate submit fence");
    }
}
}  // namespace jengine::renderer::vulkan
