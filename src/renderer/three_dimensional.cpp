#include "three_dimensional.hpp"

#include <SDL3/SDL_video.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <thread>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include "renderer/vulkan/descriptors.hpp"
#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/immediate_submit.hpp"
#include "renderer/vulkan/initializers.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/pipelines.hpp"
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
      allocator(instance.GetInstance(),
                device.GetDevice(),
                physical_device.GetPhysicalDevice(),
                VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT),
      draw_image(vulkan::CreateDrawImage(GetWindowWidth(window),
                                         GetWindowHeight(window),
                                         allocator.GetAllocator(),
                                         device.GetDevice())),
      compute_queue(device.GetVkbDevice()),
      immediate_submit(device.GetDevice(), compute_queue.GetQueueFamilyIndex(), deletion_stack),
      imgui_context(window,
                    device.GetDevice(),
                    instance.GetInstance(),
                    physical_device.GetPhysicalDevice(),
                    compute_queue.GetQueue(),
                    compute_queue.GetQueueFamilyIndex(),
                    swapchain.GetSwapchainImageFormatPtr(),
                    deletion_stack) {
    InitImgui(window);
    InitDescriptors();
    InitPipelines();
}

ThreeDimensional::~ThreeDimensional() { Destroy(); }

void ThreeDimensional::DrawFrame() {
    std::cout << "Frame " << frame_counter << std::endl;

    // imgui setup
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();

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
        command_buffer, draw_image.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vulkan::TransitionImage(
        command_buffer, swapchain_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vulkan::CopyImageBlit(command_buffer,
                          draw_image.image,
                          swapchain_image,
                          {draw_image.extent.width, draw_image.extent.height},
                          swapchain.GetExtent());

    vulkan::TransitionImage(
        command_buffer, swapchain_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ThreeDimensional::Destroy() {
    vkDeviceWaitIdle(device.GetDevice());
    while (!deletion_stack.empty()) {
        deletion_stack.top()();
        deletion_stack.pop();
    }
    global_descriptor_set_allocator.Destroy(device.GetDevice());
    vkDestroyDescriptorSetLayout(device.GetDevice(), draw_image_descriptor_layout, nullptr);
    draw_image.Destroy(device.GetDevice(), allocator.GetAllocator());
    allocator.Destroy();
    frame_in_flight_data.Destroy(device.GetDevice());
    swapchain.Destroy(device.GetDevice());
    device.Destroy();
    surface.Destroy(instance.GetInstance());
}

void ThreeDimensional::DrawBackground(VkCommandBuffer command_buffer) {
    vulkan::TransitionImage(command_buffer, draw_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    auto clear_subresource_range = vulkan::init::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    float flash = std::abs(std::sin((float)frame_counter / 100.f));
    VkClearColorValue clear_color = {0.0f, 0.0f, flash, 1.0f};

    vkCmdClearColorImage(
        command_buffer, draw_image.image, VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_subresource_range);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradient_pipeline);
    vkCmdBindDescriptorSets(command_buffer,
                            VK_PIPELINE_BIND_POINT_COMPUTE,
                            gradient_pipeline_layout,
                            0,
                            1,
                            &draw_image_descriptors,
                            0,
                            nullptr);
    vkCmdDispatch(
        command_buffer, std::ceil(draw_image.extent.width / 16.f), std::ceil(draw_image.extent.height / 16.f), 1);
}

vulkan::FrameInFlightData& ThreeDimensional::GetCurrentFrameInFlightData() {
    return frame_in_flight_data[frame_counter % frame_in_flight_data.Size()];
}

void ThreeDimensional::InitDescriptors() {
    std::vector<vulkan::DescriptorAllocator::PoolSizeRatio> pool_size_ratios{
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f},
    };
    global_descriptor_set_allocator.InitPool(device.GetDevice(), MAX_DESCRIPTOR_SETS, pool_size_ratios);

    vulkan::DescriptorLayoutBuilder layout_builder{};
    layout_builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    draw_image_descriptor_layout = layout_builder.Build(device.GetDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
    draw_image_descriptors = global_descriptor_set_allocator.Allocate(device.GetDevice(), draw_image_descriptor_layout);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    image_info.imageView = draw_image.image_view;

    VkWriteDescriptorSet draw_image_write{};
    draw_image_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    draw_image_write.pNext = nullptr;

    draw_image_write.dstSet = draw_image_descriptors;
    draw_image_write.dstBinding = 0;

    draw_image_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    draw_image_write.descriptorCount = 1;

    draw_image_write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(device.GetDevice(), 1, &draw_image_write, 0, nullptr);
}

void ThreeDimensional::InitPipelines() { InitBackgroundPipleines(); }

void ThreeDimensional::InitBackgroundPipleines() {
    VkPipelineLayoutCreateInfo compute_pipeline_layout_create_info{};
    compute_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    compute_pipeline_layout_create_info.pNext = nullptr;
    compute_pipeline_layout_create_info.pSetLayouts = &draw_image_descriptor_layout;
    compute_pipeline_layout_create_info.setLayoutCount = 1;

    if (vkCreatePipelineLayout(
            device.GetDevice(), &compute_pipeline_layout_create_info, nullptr, &gradient_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkShaderModule compute_shader_module =
        vulkan::pipelines::LoadShaderModule("shaders/gradient.comp.spv", device.GetDevice());
    if (!compute_shader_module) {
        throw std::runtime_error("Failed to create compute shader module");
    }

    VkPipelineShaderStageCreateInfo compute_pipeline_shader_stage_create_info{};
    compute_pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compute_pipeline_shader_stage_create_info.pNext = nullptr;
    compute_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compute_pipeline_shader_stage_create_info.module = compute_shader_module;
    compute_pipeline_shader_stage_create_info.pName = "main";

    VkComputePipelineCreateInfo compute_pipeline_create_info{};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.pNext = nullptr;
    compute_pipeline_create_info.layout = gradient_pipeline_layout;
    compute_pipeline_create_info.stage = compute_pipeline_shader_stage_create_info;

    if (vkCreateComputePipelines(
            device.GetDevice(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &gradient_pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute pipeline");
    }

    vkDestroyShaderModule(device.GetDevice(), compute_shader_module, nullptr);

    deletion_stack.push([&]() {
        vkDestroyPipelineLayout(device.GetDevice(), gradient_pipeline_layout, nullptr);
        vkDestroyPipeline(device.GetDevice(), gradient_pipeline, nullptr);
    });
}

void ThreeDimensional::InitImgui(SDL_Window* window) {
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100}};

    VkDescriptorPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_create_info.maxSets = 100;
    pool_create_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_create_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imgui_descriptor_pool;

    if (vkCreateDescriptorPool(device.GetDevice(), &pool_create_info, nullptr, &imgui_descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create imgui descriptor pool");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL3_InitForVulkan(window);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = instance.GetInstance();
    init_info.PhysicalDevice = physical_device.GetPhysicalDevice();
    init_info.Device = device.GetDevice();
    init_info.QueueFamily = graphics_queue.GetQueueFamilyIndex();
    init_info.Queue = graphics_queue.GetQueue();
    init_info.DescriptorPool = imgui_descriptor_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;

    init_info.UseDynamicRendering = true;

    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        swapchain.GetSwapchainImageFormatPtr();

    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    SDL_SetEventFilter(&ThreeDimensional::ImguiSDLEventFilter, nullptr);

    deletion_stack.push([this, imgui_descriptor_pool]() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(device.GetDevice(), imgui_descriptor_pool, nullptr);
    });
}

bool ThreeDimensional::ImguiSDLEventFilter(void* user_data, SDL_Event* event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
            event->type == SDL_EVENT_MOUSE_MOTION || event->type == SDL_EVENT_MOUSE_WHEEL ||
            event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
            return false;
        }
    }
    return true;
}
}  // namespace jengine::renderer
