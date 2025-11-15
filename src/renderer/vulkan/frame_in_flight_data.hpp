#pragma once

#include <vulkan/vulkan_core.h>

#include <iostream>
#include <stdexcept>

#include "renderer/vulkan/deletion_stack.hpp"
#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan {
struct FrameInFlightData {
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore image_available_semaphore;
    VkFence render_in_progress_fence;
};

template <size_t S>
class FrameInFlightDataContainer {
  public:
    FrameInFlightDataContainer(uint32_t graphics_queue_family_index, VkDevice device, DeletionStack& deletion_stack);

    FrameInFlightData& operator[](size_t index) { return frame_in_flight_data[index]; }

    constexpr size_t Size() const { return S; }

    // does not call device wait idle
    void Destroy(VkDevice device);

  private:
    FrameInFlightData frame_in_flight_data[S];
};

template <size_t S>
FrameInFlightDataContainer<S>::FrameInFlightDataContainer(uint32_t graphics_queue_family_index,
                                                          VkDevice device,
                                                          DeletionStack& deletion_stack) {
    VkCommandPoolCreateInfo command_pool_create_info =
        init::CommandPoolCreateInfo(graphics_queue_family_index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VkFenceCreateInfo fence_create_info = init::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

    VkSemaphoreCreateInfo semaphore_create_info = init::SemaphoreCreateInfo(0);

    for (size_t i = 0; i < S; i++) {
        if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &frame_in_flight_data[i].command_pool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool");
        }

        VkCommandBufferAllocateInfo command_buffer_allocate_info =
            vulkan::init::PrimaryCommandBufferAllocateInfo(frame_in_flight_data[i].command_pool, 1);

        if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &frame_in_flight_data[i].command_buffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer");
        }

        if (vkCreateSemaphore(
                device, &semaphore_create_info, nullptr, &frame_in_flight_data[i].image_available_semaphore) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create image available semaphore");
        }
        if (vkCreateFence(device, &fence_create_info, nullptr, &frame_in_flight_data[i].render_in_progress_fence) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create render in process fence");
        }
    }

    deletion_stack.Push([this, device]() { Destroy(device); });
}

template <size_t S>
void FrameInFlightDataContainer<S>::Destroy(VkDevice device) {
    std::cout << "Destroying frame in flight data" << std::endl;
    for (auto& frame_in_flight_data : frame_in_flight_data) {
        vkDestroyCommandPool(device, frame_in_flight_data.command_pool, nullptr);
        vkDestroySemaphore(device, frame_in_flight_data.image_available_semaphore, nullptr);
        vkDestroyFence(device, frame_in_flight_data.render_in_progress_fence, nullptr);
    }
}

}  // namespace jengine::renderer::vulkan
