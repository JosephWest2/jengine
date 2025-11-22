#include "three_dimensional.hpp"

#include <SDL3/SDL_video.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>
#include <vulkan/vulkan_raii.hpp>

#include "imgui.h"
#include "renderer/vulkan/debug.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
#include "vulkan/vulkan.hpp"
#include "window.hpp"

namespace jengine::renderer {

ThreeDimensional::ThreeDimensional(SDL_Window* window, std::string_view app_name)
    : instance(app_name.data()),
      debug_messenger(instance.GetInstance()),
      physical_device(instance.GetInstance()),
      device(physical_device),
      surface(window, instance.GetInstance()),
      swapchain(GetWindowWidth(window),
                GetWindowHeight(window),
                physical_device,
                device.GetDevice(),
                surface.GetSurface()),
      graphics_queue(physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics), *device.GetDevice()),
      frame_in_flight_data(physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics), device.GetDevice()),
      immediate_submit(device.GetDevice(), graphics_queue.GetQueueFamilyIndex()),
      allocator(instance.GetInstance(),
                device.GetDevice(),
                physical_device.GetPhysicalDevice(),
                VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT),
      draw_image(vk::Extent3D{(uint32_t)GetWindowWidth(window), (uint32_t)GetWindowHeight(window), 1},
                 vk::Format::eR16G16B16A16Sfloat,
                 vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc |
                     vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment,
                 device.GetDevice(),
                 allocator.GetAllocator()),
      imgui_context(window,
                    device.GetDevice(),
                    instance.GetInstance(),
                    physical_device.GetPhysicalDevice(),
                    graphics_queue.GetQueue(),
                    graphics_queue.GetQueueFamilyIndex(),
                    swapchain.GetSwapchainImageFormat()),
      descriptor_manager(device.GetDevice(), draw_image.GetImageView()),
      pipeline_manager(device.GetDevice(),
                       draw_image.GetFormat(),
                       descriptor_manager.GetDrawImageDescriptorLayoutPtr()) {}

ThreeDimensional::~ThreeDimensional() { device.GetDevice().waitIdle(); }

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

    device.WaitIdle();
    device.WaitForFences({GetCurrentFrameInFlightData().GetRenderInProgressFence()});
    device.ResetFences({GetCurrentFrameInFlightData().GetRenderInProgressFence()});

    auto result = device.GetDevice().acquireNextImage2KHR(vk::AcquireNextImageInfoKHR{
        .swapchain = swapchain.GetSwapchain(),
        .timeout = TIMEOUT_ONE_SECOND,
        .semaphore = GetCurrentFrameInFlightData().GetImageAvailableSemaphore(),
    });
    if (!result.has_value()) {
        throw std::runtime_error("Failed to acquire next image");
    }
    uint32_t swapchain_image_index = result.value;

    auto& swapchain_image = swapchain.GetSwapchainImages()[swapchain_image_index];
    auto& swapchain_image_view = swapchain.GetSwapchainImageViews()[swapchain_image_index];

    vk::CommandBuffer& command_buffer = GetCurrentFrameInFlightData().GetCommandBuffer();

    command_buffer.reset();

    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT means the command buffer will be used only for one submit per frame
    // (per reset) it allows vulkan to potentially optimize
    command_buffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    DrawBackground(command_buffer);
    vulkan::TransitionImage(
        command_buffer, draw_image.GetImage(), vk::ImageLayout::eGeneral, vk::ImageLayout::eColorAttachmentOptimal);

    DrawGeometry(command_buffer);

    vulkan::TransitionImage(command_buffer,
                            draw_image.GetImage(),
                            vk::ImageLayout::eColorAttachmentOptimal,
                            vk::ImageLayout::eTransferSrcOptimal);
    vulkan::TransitionImage(
        command_buffer, swapchain_image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    vulkan::CopyImageBlit(command_buffer,
                          draw_image.GetImage(),
                          swapchain_image,
                          {draw_image.GetExtent().width, draw_image.GetExtent().height},
                          swapchain.GetExtent());

    vulkan::TransitionImage(command_buffer,
                            swapchain_image,
                            vk::ImageLayout::eTransferDstOptimal,
                            vk::ImageLayout::eColorAttachmentOptimal);
    imgui_context.Draw(command_buffer, swapchain_image_view, swapchain.GetExtent());

    vulkan::TransitionImage(
        command_buffer, swapchain_image, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

    command_buffer.end();

    vk::CommandBufferSubmitInfo command_buffer_submit_info{.commandBuffer = command_buffer};
    vk::SemaphoreSubmitInfo wait_semaphore_submit_info{
        .semaphore = GetCurrentFrameInFlightData().GetImageAvailableSemaphore(),
        .stageMask = vk::PipelineStageFlagBits2::eAllGraphics};
    vk::SemaphoreSubmitInfo signal_semaphore_submit_info{
        .semaphore = swapchain.GetImageRenderFinishedSemaphores()[swapchain_image_index],
        .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
    vk::SubmitInfo2 submit_info = vulkan::init::SubmitInfo2(
        &command_buffer_submit_info, &wait_semaphore_submit_info, &signal_semaphore_submit_info);

    graphics_queue.GetQueue().submit2(submit_info, GetCurrentFrameInFlightData().GetRenderInProgressFence());

    vk::PresentInfoKHR present_info{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*swapchain.GetImageRenderFinishedSemaphores()[swapchain_image_index],
        .swapchainCount = 1,
        .pSwapchains = swapchain.GetSwapchainPtr(),
        .pImageIndices = &swapchain_image_index,
    };

    if (graphics_queue.GetQueue().presentKHR(present_info) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present queue");
    }

    frame_counter++;

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

void ThreeDimensional::DrawBackground(vk::CommandBuffer command_buffer) {
    vulkan::TransitionImage(
        command_buffer, draw_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

    vk::ImageSubresourceRange clear_subresource_range =
        vulkan::init::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor);

    float flash = std::abs(std::sin((float)frame_counter / 100.f));

    vk::ClearColorValue clear_color{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};

    command_buffer.clearColorImage(
        draw_image.GetImage(), vk::ImageLayout::eGeneral, clear_color, clear_subresource_range);

    command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute,
                                pipeline_manager.GetGradientAndSkyPipeline().GetGradientPipeline());

    command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeline_manager.GetGradientAndSkyPipeline().GetPipelineLayout(), 0, )
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
                       sizeof(vulkan::pipelines::GradientPipeline::ComputePushConstants),
                       &pipeline_manager.GetGradientAndSkyPipeline().GradientPushConstants());

    vkCmdDispatch(command_buffer,
                  std::ceil(draw_image.GetExtent().width / 16.f),
                  std::ceil(draw_image.GetExtent().height / 16.f),
                  1);
}

vulkan::FrameInFlightData& ThreeDimensional::GetCurrentFrameInFlightData() {
    return frame_in_flight_data[frame_counter % frame_in_flight_data.Size()];
}

void ThreeDimensional::DrawGeometry(vk::CommandBuffer command_buffer) {
    vk::RenderingAttachmentInfo color_attachment = vulkan::init::RenderingAttachmentInfo(
        draw_image.GetImageView(), std::nullopt, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vk::RenderingInfo rendering_info = vulkan::init::RenderingInfo(
        {draw_image.GetExtent().width, draw_image.GetExtent().height}, &color_attachment, nullptr);

    vkCmdBeginRendering(command_buffer, &rendering_info);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_manager.GetTrianglePipeline());

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = draw_image.GetExtent().width;
    viewport.height = draw_image.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {draw_image.GetExtent().width, draw_image.GetExtent().height};

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    vkCmdEndRendering(command_buffer);
}
}  // namespace jengine::renderer
