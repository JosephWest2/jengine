#include "three_dimensional.hpp"

#include <SDL3/SDL_video.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>

#include "imgui.h"
#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/immediate_submit.hpp"
#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
#include "window.hpp"

namespace jengine::renderer {

ThreeDimensional::ThreeDimensional(SDL_Window* window, vulkan::Instance& instance)
    : instance(instance),
      surface(window, instance.GetInstance()),
      physical_device(surface.GetSurface(), instance.VkbInstance()),
      device(physical_device.GetVkbPhysicalDevice()),
      swapchain(GetWindowWidth(window),
                GetWindowHeight(window),
                physical_device.GetPhysicalDevice(),
                device.GetDevice(),
                surface.GetSurface()),
      graphics_queue(device.GetVkbDevice()),
      frame_in_flight_data(graphics_queue.GetQueueFamilyIndex(), device.GetDevice()),
      graphics_queue_2(device.GetVkbDevice()),
      immediate_submit(device.GetDevice(), graphics_queue_2.GetQueueFamilyIndex()),
      allocator(instance.GetInstance(),
                device.GetDevice(),
                physical_device.GetPhysicalDevice(),
                VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT),
      draw_image(VkExtent3D{(uint32_t)GetWindowWidth(window), (uint32_t)GetWindowHeight(window), 1},
                 VK_FORMAT_R16G16B16A16_SFLOAT,
                 VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                 device.GetDevice(),
                 allocator.GetAllocator()),
      imgui_context(window,
                    device.GetDevice(),
                    instance.GetInstance(),
                    physical_device.GetPhysicalDevice(),
                    graphics_queue.GetQueue(),
                    graphics_queue.GetQueueFamilyIndex(),
                    swapchain.GetSwapchainImageFormatPtr()),
      descriptor_manager(device.GetDevice(), draw_image.GetImageView()),
      pipeline_manager(device.GetDevice(),
                       draw_image.GetFormat(),
                       descriptor_manager.GetDrawImageDescriptorLayoutPtr()) {}

ThreeDimensional::~ThreeDimensional() { vkDeviceWaitIdle(device.GetDevice()); }

void ThreeDimensional::DrawFrame() {
    std::cout << "Frame " << frame_counter << std::endl;

    // imgui setup
    imgui_context.NewFrame([this]() {
        auto pipeline_name = pipeline_manager.GetGradientAndSkyPipeline().SelectedPipelineString();

        ImGui::Text("Selected pipeline: %s", pipeline_name.c_str());

        ImGui::InputFloat4("sky1", (float*)&pipeline_manager.GetGradientAndSkyPipeline().SkyPushConstants().data1);
        ImGui::InputFloat4("sky2", (float*)&pipeline_manager.GetGradientAndSkyPipeline().SkyPushConstants().data2);
        ImGui::InputFloat4("sky3", (float*)&pipeline_manager.GetGradientAndSkyPipeline().SkyPushConstants().data3);
        ImGui::InputFloat4("sky4", (float*)&pipeline_manager.GetGradientAndSkyPipeline().SkyPushConstants().data4);

        ImGui::InputFloat4("gradient1",
                           (float*)&pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants().data1);
        ImGui::InputFloat4("gradient2",
                           (float*)&pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants().data2);
        ImGui::InputFloat4("gradient3",
                           (float*)&pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants().data3);
        ImGui::InputFloat4("gradient4",
                           (float*)&pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants().data4);
    });

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

    auto& swapchain_image = swapchain.GetSwapchainImages()[swapchain_image_index];
    auto& swapchain_image_view = swapchain.GetSwapchainImageViews()[swapchain_image_index];

    VkCommandBuffer& command_buffer = GetCurrentFrameInFlightData().command_buffer;

    if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }

    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT means the command buffer will be used only for one submit per frame
    // (per reset) it allows vulkan to potentially optimize
    VkCommandBufferBeginInfo command_buffer_begin_info =
        vulkan::init::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    DrawBackground(command_buffer);
    vulkan::TransitionImage(
        command_buffer, draw_image.GetImage(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    DrawGeometry(command_buffer);

    vulkan::TransitionImage(
        command_buffer, draw_image.GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vulkan::TransitionImage(
        command_buffer, swapchain_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vulkan::CopyImageBlit(command_buffer,
                          draw_image.GetImage(),
                          swapchain_image,
                          {draw_image.GetExtent().width, draw_image.GetExtent().height},
                          swapchain.GetExtent());

    vulkan::TransitionImage(command_buffer,
                            swapchain_image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    imgui_context.Draw(command_buffer, swapchain_image_view, swapchain.GetExtent());

    vulkan::TransitionImage(
        command_buffer, swapchain_image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }

    VkCommandBufferSubmitInfo command_buffer_submit_info = vulkan::init::CommandBufferSubmitInfo(command_buffer);
    VkSemaphoreSubmitInfo wait_semaphore_submit_info = vulkan::init::SemaphoreSubmitInfo(
        GetCurrentFrameInFlightData().image_available_semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    VkSemaphoreSubmitInfo signal_semaphore_submit_info =
        vulkan::init::SemaphoreSubmitInfo(swapchain.GetImageRenderFinishedSemaphores()[swapchain_image_index],
                                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
    VkSubmitInfo2 submit_info = vulkan::init::SubmitInfo2(
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

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

void ThreeDimensional::DrawBackground(VkCommandBuffer command_buffer) {
    vulkan::TransitionImage(command_buffer, draw_image.GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    auto clear_subresource_range = vulkan::init::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    float flash = std::abs(std::sin((float)frame_counter / 100.f));

    VkClearColorValue clear_color = {{0.0f, 0.0f, flash, 1.0f}};

    vkCmdClearColorImage(
        command_buffer, draw_image.GetImage(), VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_subresource_range);

    vkCmdBindPipeline(command_buffer,
                      VK_PIPELINE_BIND_POINT_COMPUTE,
                      pipeline_manager.GetGradientAndSkyPipeline().GetGradientPipeline());
    vkCmdBindDescriptorSets(command_buffer,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            pipeline_manager.GetGradientAndSkyPipeline().GetPipelineLayout(),
                            0,
                            1,
                            descriptor_manager.GetDrawImageDescriptorSetPtr(),
                            0,
                            nullptr);

    vkCmdPushConstants(command_buffer,
                       pipeline_manager.GetGradientAndSkyPipeline().GetPipelineLayout(),
                       VK_SHADER_STAGE_COMPUTE_BIT,
                       0,
                       sizeof(vulkan::pipelines::GradientAndSkyPipeline::ComputePushConstants),
                       &pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants());

    vkCmdDispatch(command_buffer,
                  std::ceil(draw_image.GetExtent().width / 16.f),
                  std::ceil(draw_image.GetExtent().height / 16.f),
                  1);
}

vulkan::FrameInFlightData& ThreeDimensional::GetCurrentFrameInFlightData() {
    return frame_in_flight_data[frame_counter % frame_in_flight_data.Size()];
}

void ThreeDimensional::DrawGeometry(VkCommandBuffer command_buffer) {
    VkRenderingAttachmentInfo color_attachment = vulkan::init::RenderingAttachmentInfo(
        draw_image.GetImageView(), std::nullopt, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo rendering_info = vulkan::init::RenderingInfo(
        {draw_image.GetExtent().width, draw_image.GetExtent().height}, &color_attachment, nullptr);

    vkCmdBeginRendering(command_buffer, &rendering_info);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_manager.GetTrianglePipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = draw_image.GetExtent().width;
    viewport.height = draw_image.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {draw_image.GetExtent().width, draw_image.GetExtent().height};

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRendering(command_buffer);
}
}  // namespace jengine::renderer
