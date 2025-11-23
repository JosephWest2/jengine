#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {
class FrameInFlightData {
  public:
    FrameInFlightData(uint32_t graphics_queue_family_index, const vk::raii::Device& device);

    vk::CommandBuffer& GetCommandBuffer() { return command_buffer; }
    vk::raii::Semaphore& GetImageAvailableSemaphore() { return image_available_semaphore; }
    vk::raii::Fence& GetRenderInProgressFence() { return render_in_progress_fence; }
    vk::raii::CommandPool& GetCommandPool() { return command_pool; }

  private:
    vk::raii::CommandPool command_pool;
    vk::CommandBuffer command_buffer;
    vk::raii::Semaphore image_available_semaphore;
    vk::raii::Fence render_in_progress_fence;
};

template <size_t SIZE>
class FrameInFlightDataContainer {
  public:
    FrameInFlightDataContainer(uint32_t graphics_queue_family_index, const vk::raii::Device& device) {
        for (size_t i = 0; i < SIZE; i++) {
            data[i] = FrameInFlightData(graphics_queue_family_index, device);
        }
    }
    FrameInFlightData& operator[](size_t index) { return data[index].value(); }
    constexpr size_t Size() { return SIZE; }

  private:
    std::array<std::optional<FrameInFlightData>, SIZE> data;
};
}  // namespace jengine::renderer::vulkan
