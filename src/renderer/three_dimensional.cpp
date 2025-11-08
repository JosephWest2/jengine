#include "three_dimensional.hpp"

#include <SDL3/SDL_video.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>

#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/physical_device.hpp"

namespace jengine::renderer {

ThreeDimensional::ThreeDimensional(SDL_Window* window, vulkan::Instance& instance)
    : instance(instance),
      surface(window, instance.GetInstance()),
      physical_device(surface.GetSurface(), instance.VkbInstance()),
      device(physical_device.GetVkbPhysicalDevice()),
      swapchain(window, physical_device.GetPhysicalDevice(), device.GetDevice(), surface.GetSurface()),
      graphics_queue(device.GetVkbDevice()),
      frame_in_flight_data(graphics_queue.GetQueueFamilyIndex(), device.GetDevice()) {
    std::cout << "swapchain.GetSwapchain(): " << swapchain.GetSwapchain() << std::endl;
}

ThreeDimensional::~ThreeDimensional() {
    Cleanup();
}

void ThreeDimensional::DrawFrame() {
    std::cout << "Frame " << frame_counter << std::endl;
    std::cout << "swapchain.GetSwapchain(): " << swapchain.GetSwapchain() << std::endl;
    if (vkWaitForFences(
            device.GetDevice(), 1, &GetCurrentFrameInFlightData().render_in_progress_fence, true, 1000000000) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for render in process fence");
    }

    if (vkResetFences(device.GetDevice(), 1, &GetCurrentFrameInFlightData().render_in_progress_fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset render in process fence");
    }
    uint32_t swapchain_image_index;
    if (vkAcquireNextImageKHR(device.GetDevice(),
                              swapchain.GetSwapchain(),
                              1000000000,
                              GetCurrentFrameInFlightData().image_available_semaphore,
                              nullptr,
                              &swapchain_image_index) != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire next swapchain image index");
    }
    VkCommandBuffer& command_buffer = GetCurrentFrameInFlightData().command_buffer;

    if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT means the command buffer will be used only for one submit per frame
    // (per reset) it allows vulkan to potentially optimize
    VkCommandBufferBeginInfo command_buffer_begin_info =
        vulkan::InitCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    vulkan::TransitionImage(command_buffer,
                            swapchain.GetSwapchainImages()[swapchain_image_index],
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_GENERAL);

    auto clear_subresource_range = vulkan::InitImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    float flash = std::abs(std::sin((float)frame_counter / 100.f));
    VkClearColorValue clear_color = {0.0f, 0.0f, flash, 1.0f};

    vkCmdClearColorImage(command_buffer,
                         swapchain.GetSwapchainImages()[swapchain_image_index],
                         VK_IMAGE_LAYOUT_GENERAL,
                         &clear_color,
                         1,
                         &clear_subresource_range);

    vulkan::TransitionImage(command_buffer,
                            swapchain.GetSwapchainImages()[swapchain_image_index],
                            VK_IMAGE_LAYOUT_GENERAL,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }

    VkCommandBufferSubmitInfo command_buffer_submit_info = vulkan::InitCommandBufferSubmitInfo(command_buffer);
    VkSemaphoreSubmitInfo wait_semaphore_submit_info = vulkan::InitSemaphoreSubmitInfo(
        GetCurrentFrameInFlightData().image_available_semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    VkSemaphoreSubmitInfo signal_semaphore_submit_info =
        vulkan::InitSemaphoreSubmitInfo(swapchain.GetImageRenderFinishedSemaphores()[swapchain_image_index],
                                        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
    VkSubmitInfo2 submit_info = vulkan::InitSubmitInfo2(
        &command_buffer_submit_info, &wait_semaphore_submit_info, &signal_semaphore_submit_info);

    if (vkQueueSubmit2(
            graphics_queue.GetQueue(), 1, &submit_info, GetCurrentFrameInFlightData().render_in_progress_fence) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to submit queue");
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pSwapchains = swapchain.GetSwapchainPtr();
    present_info.swapchainCount = 1;

    present_info.pWaitSemaphores = &swapchain.GetImageRenderFinishedSemaphores()[swapchain_image_index];
    present_info.waitSemaphoreCount = 1;

    present_info.pImageIndices = &swapchain_image_index;

    if (vkQueuePresentKHR(graphics_queue.GetQueue(), &present_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to present queue");
    }

    frame_counter++;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ThreeDimensional::Cleanup() {
    vkDeviceWaitIdle(device.GetDevice());
    frame_in_flight_data.Destroy(device.GetDevice());
    deletion_queue.Flush();
    swapchain.Destroy(device.GetDevice());
    device.Destroy();
    surface.Destroy(instance.GetInstance());
}
}  // namespace jengine::renderer
