#include "three_dimensional.hpp"

#include <SDL3/SDL_video.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <print>
#include <thread>
#include <vulkan/vulkan_raii.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include "renderer/gltf/load_meshes.hpp"
#include "renderer/vulkan/buffers/mesh_buffers.hpp"
#include "renderer/vulkan/debug.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/swapchain.hpp"
#include "vulkan/vulkan.hpp"
#include "window.hpp"

namespace jengine::renderer {

ThreeDimensional::ThreeDimensional(SDL_Window* window, std::string_view app_name)
    : instance(app_name.data()),
      debug_messenger(instance.GetInstance()),
      physical_device(instance.GetInstance()),
      device(physical_device),
      surface(window, *instance.GetInstance()),
      swapchain(GetWindowWidth(window),
                GetWindowHeight(window),
                physical_device,
                device.GetDevice(),
                surface.GetSurface()),
      graphics_queue(physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics), device.GetDevice()),
      frame_in_flight_data(physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics), device.GetDevice()),
      immediate_submit(device.GetDevice(), graphics_queue.GetQueueFamilyIndex()),
      allocator(instance.GetInstance(),
                device.GetDevice(),
                physical_device.GetPhysicalDevice(),
                VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT),
      draw_image(vk::Extent3D{(uint32_t)GetWindowWidth(window), (uint32_t)GetWindowHeight(window), 1},
                 vk::Format::eR16G16B16A16Sfloat,
                 vk::ImageAspectFlagBits::eColor,
                 vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc |
                     vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment,
                 device.GetDevice(),
                 allocator.GetAllocator()),
      depth_image(vk::Extent3D{(uint32_t)GetWindowWidth(window), (uint32_t)GetWindowHeight(window), 1},
                  vk::Format::eD32Sfloat,
                  vk::ImageAspectFlagBits::eDepth,
                  vk::ImageUsageFlagBits::eDepthStencilAttachment,
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
                       depth_image.GetFormat(),
                       descriptor_manager.GetDrawImageDescriptorLayoutPtr()),
      mesh_buffers(vulkan::buffers::test_indices,
                   vulkan::buffers::test_vertices,
                   allocator.GetAllocator(),
                   immediate_submit,
                   device.GetDeviceHandle(),
                   graphics_queue.GetQueue()),
      camera(75.f, (float)GetWindowWidth(window) / (float)GetWindowHeight(window)) {
    LoadMeshAsset("../assets/basicmesh.glb");
}

ThreeDimensional::~ThreeDimensional() { device.GetDevice().waitIdle(); }

void ThreeDimensional::DrawFrame() {
    std::println("Frame {}, rate: {}", frame_counter, FrameRate());
    last_frame_start_time = std::chrono::steady_clock::now();

    if (resize_requested) {
        return;
    }

    // imgui setup
    imgui_context.NewFrame([this]() {
        if (ImGui::Begin("background")) {
            auto pipeline_name = pipeline_manager.GetSelectedSharedComputePipelineName();

            ImGui::Text("Selected pipeline: %s", pipeline_name.data());

            if (ImGui::Button("Select Sky")) {
                pipeline_manager.SetSelectedSharedComputePipeline(
                    vulkan::pipelines::Manager::SelectedSharedComputePipeline::SKY);
            }
            if (ImGui::Button("Select Gradient")) {
                pipeline_manager.SetSelectedSharedComputePipeline(
                    vulkan::pipelines::Manager::SelectedSharedComputePipeline::GRADIENT);
            }

            ImGui::InputFloat4("ComputePushConstant1",
                               (float*)&pipeline_manager.GetSharedComputePipelineLayout().GetPushConstants().data1);
            ImGui::InputFloat4("ComputePushConstant2",
                               (float*)&pipeline_manager.GetSharedComputePipelineLayout().GetPushConstants().data2);
            ImGui::InputFloat4("ComputePushConstant3",
                               (float*)&pipeline_manager.GetSharedComputePipelineLayout().GetPushConstants().data3);
            ImGui::InputFloat4("ComputePushConstant4",
                               (float*)&pipeline_manager.GetSharedComputePipelineLayout().GetPushConstants().data4);
        }
        ImGui::End();
    });

    device.WaitIdle();
    device.WaitForFences({GetCurrentFrameInFlightData().GetRenderInProgressFence()});
    device.ResetFences({GetCurrentFrameInFlightData().GetRenderInProgressFence()});

    auto result = device.GetDevice().acquireNextImage2KHR(vk::AcquireNextImageInfoKHR{
        .swapchain = swapchain.GetSwapchain(),
        .timeout = TIMEOUT_ONE_SECOND,
        .semaphore = GetCurrentFrameInFlightData().GetImageAvailableSemaphore(),
        .deviceMask = 1,
    });
    if (result.result == vk::Result::eErrorOutOfDateKHR) {
        resize_requested = true;
        return;
    }
    if (!result.has_value()) {
        throw std::runtime_error("Failed to acquire next image");
    }
    uint32_t swapchain_image_index = result.value;

    auto& swapchain_image = swapchain.GetImage(swapchain_image_index);
    auto& swapchain_image_view = swapchain.GetImageView(swapchain_image_index);

    const vk::CommandBuffer& command_buffer = GetCurrentFrameInFlightData().GetCommandBuffer();

    command_buffer.reset();

    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT means the command buffer will be used only for one submit per frame
    // (per reset) it allows vulkan to potentially optimize
    command_buffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    DrawBackground(command_buffer);
    vulkan::TransitionImage(
        command_buffer, draw_image.GetImage(), vk::ImageLayout::eGeneral, vk::ImageLayout::eColorAttachmentOptimal);

    vulkan::TransitionImage(
        command_buffer, depth_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal);
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
        .semaphore = swapchain.GetImageRenderFinishedSemaphore(swapchain_image_index),
        .stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    };
    vk::SubmitInfo2 submit_info = vulkan::init::SubmitInfo2(
        &command_buffer_submit_info, &wait_semaphore_submit_info, &signal_semaphore_submit_info);

    graphics_queue.GetQueue().submit2(submit_info, GetCurrentFrameInFlightData().GetRenderInProgressFence());

    vk::PresentInfoKHR present_info{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &swapchain.GetImageRenderFinishedSemaphore(swapchain_image_index),
        .swapchainCount = 1,
        .pSwapchains = swapchain.GetSwapchainPtr(),
        .pImageIndices = &swapchain_image_index,
    };

    {
        auto result = graphics_queue.GetQueue().presentKHR(present_info);
        if (result == vk::Result::eErrorOutOfDateKHR) {
            resize_requested = true;
            return;
        } else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present queue");
        }
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

    vk::ClearColorValue clear_color{std::array<float, 4>{0.0f, 0.0f, flash, 1.0f}};

    command_buffer.clearColorImage(
        draw_image.GetImage(), vk::ImageLayout::eGeneral, clear_color, clear_subresource_range);

    command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline_manager.GetSelectedSharedComputePipeline());

    command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                      pipeline_manager.GetSharedComputePipelineLayout().GetPipelineLayout(),
                                      0,
                                      1,
                                      descriptor_manager.GetDrawImageDescriptorSetPtr(),
                                      0,
                                      nullptr);

    auto push_constants_vec = pipeline_manager.GetSharedComputePipelineLayout().GetPushConstantsVec();
    command_buffer.pushConstants(pipeline_manager.GetSharedComputePipelineLayout().GetPipelineLayout(),
                                 vk::ShaderStageFlagBits::eCompute,
                                 0,
                                 vk::ArrayProxy<const glm::vec4>{push_constants_vec});

    command_buffer.dispatch(
        std::ceil(draw_image.GetExtent().width / 16.f), std::ceil(draw_image.GetExtent().height / 16.f), 1);
}

vulkan::FrameInFlightData& ThreeDimensional::GetCurrentFrameInFlightData() {
    return frame_in_flight_data[frame_counter % frame_in_flight_data.Size()];
}

void ThreeDimensional::DrawGeometry(vk::CommandBuffer command_buffer) {
    vk::RenderingAttachmentInfo color_attachment = vulkan::init::RenderingAttachmentInfo(
        draw_image.GetImageView(), std::nullopt, vk::ImageLayout::eColorAttachmentOptimal);
    vk::RenderingAttachmentInfo depth_attachment =
        vulkan::init::DepthAttachmentInfo(depth_image.GetImageView(), vk::ImageLayout::eDepthAttachmentOptimal);
    vk::RenderingInfo rendering_info = vulkan::init::RenderingInfo(
        {draw_image.GetExtent().width, draw_image.GetExtent().height}, &color_attachment, &depth_attachment);

    command_buffer.beginRendering(rendering_info);

    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_manager.GetMeshPipeline());

    vk::Viewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(draw_image.GetExtent().width),
        .height = static_cast<float>(draw_image.GetExtent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    command_buffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{
        .offset = {0, 0},
        .extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)},
    };

    command_buffer.setScissor(0, 1, &scissor);

    // auto rotation = glm::rotate(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
    // auto translation = glm::translate(glm::vec3(0.f, 0.f, -3.f));
    // auto perspective = glm::perspective(glm::radians(90.f), 4.f / 3.f, 1000.f, 0.1f);
    // perspective[1][1] *= -1.f;

    vulkan::buffers::MeshDrawPushConstants push_constants{
        .world_matrix = camera.ProjectionMatrix() * camera.ViewMatrix(),
        .vertex_buffer_address = loaded_mesh_assets[2].mesh_buffers.GetVertexBufferAddress(),
    };

    command_buffer.pushConstants(pipeline_manager.GetMeshPipelineLayout(),
                                 vk::ShaderStageFlagBits::eVertex,
                                 0,
                                 sizeof(push_constants),
                                 &push_constants);
    command_buffer.bindIndexBuffer(
        loaded_mesh_assets[2].mesh_buffers.GetIndexBuffer().GetBuffer(), 0, vk::IndexType::eUint32);

    command_buffer.drawIndexed(
        loaded_mesh_assets[2].surfaces[0].index_count, 1, loaded_mesh_assets[2].surfaces[0].start_index, 0, 0);

    command_buffer.endRendering();
}
void ThreeDimensional::LoadMeshAsset(std::string_view path) {
    std::vector<gltf::MeshAsset> mesh_assets = gltf::LoadMeshes(path);
    loaded_mesh_assets.reserve(loaded_mesh_assets.size() + mesh_assets.size());
    for (const auto& mesh_asset : mesh_assets) {
        loaded_mesh_assets.push_back(
            GPULoadedMeshAsset{.name = mesh_asset.name,
                               .surfaces = mesh_asset.surfaces,
                               .mesh_buffers = vulkan::buffers::MeshBuffers(mesh_asset.indices,
                                                                            mesh_asset.vertices,
                                                                            allocator.GetAllocator(),
                                                                            immediate_submit,
                                                                            device.GetDeviceHandle(),
                                                                            graphics_queue.GetQueue()),
                               .instances = {}});
    }
}
void ThreeDimensional::HandleResize(uint32_t new_width, uint32_t new_height) {
    std::println("swapchain resized");
    camera.SetAspectRatio(static_cast<float>(new_width) / static_cast<float>(new_height));
    swapchain.Rebuild(new_width, new_height, physical_device, device.GetDevice(), surface.GetSurface());
    resize_requested = false;
}
}  // namespace jengine::renderer
