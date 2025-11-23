#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <vulkan/vulkan_raii.hpp>

#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {
class FrameInFlightData {
  public:
    FrameInFlightData(uint32_t graphics_queue_family_index, const vk::raii::Device& device);

    const vk::CommandBuffer& GetCommandBuffer() { return *command_buffer; }
    const vk::Semaphore& GetImageAvailableSemaphore() { return *image_available_semaphore; }
    const vk::Fence& GetRenderInProgressFence() { return *render_in_progress_fence; }
    const vk::CommandPool& GetCommandPool() { return *command_pool; }

  private:
    vk::raii::CommandPool command_pool;
    vk::raii::Semaphore image_available_semaphore;
    vk::raii::Fence render_in_progress_fence;
    vk::raii::CommandBuffer command_buffer;
};

template <size_t SIZE>
class FrameInFlightDataContainer {
  public:
    FrameInFlightDataContainer(uint32_t graphics_queue_family_index, const vk::raii::Device& device) {
        for (size_t i = 0; i < SIZE; i++) {
            data[i] = FrameInFlightData(graphics_queue_family_index, device);
        }
    }
    FrameInFlightData& operator[](size_t index) {
        if (index >= SIZE) {
            throw std::out_of_range("Index out of range");
        }
        if (!data[index].has_value()) {
            throw std::runtime_error("Frame in flight data not initialized");
        }
        return data[index].value();
    }
    constexpr size_t Size() { return SIZE; }

  private:
    std::array<std::optional<FrameInFlightData>, SIZE> data;
};
}  // namespace jengine::renderer::vulkan
