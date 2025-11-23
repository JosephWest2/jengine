#include "frame_in_flight_data.hpp"
#include <vulkan/vulkan_raii.hpp>

namespace jengine::renderer::vulkan {
FrameInFlightData::FrameInFlightData(uint32_t graphics_queue_family_index, const vk::raii::Device& device)
    : command_pool(
          device,
          {.flags = vk::CommandPoolCreateFlagBits::eTransient, .queueFamilyIndex = graphics_queue_family_index}),
      image_available_semaphore(device, vk::SemaphoreCreateInfo{}),
      render_in_progress_fence(device, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled}), command_buffer(nullptr) {
    auto buffers = device.allocateCommandBuffers({
        .commandPool = command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    });
    command_buffer = std::move(buffers.front());
}
}  // namespace jengine::renderer::vulkan
