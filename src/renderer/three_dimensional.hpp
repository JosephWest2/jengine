#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <functional>
#include <vulkan/vulkan_raii.hpp>

#include "SDL3/SDL_video.h"
#include "renderer/base.hpp"
#include "renderer/imgui/context.hpp"
#include "renderer/vulkan/debug.hpp"
#include "renderer/vulkan/descriptors/manager.hpp"
#include "renderer/vulkan/device.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/immediate_submit.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/memory_allocator.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/pipelines/manager.hpp"
#include "renderer/vulkan/surface.hpp"
#include "renderer/vulkan/swapchain.hpp"

namespace jengine::renderer {

class ThreeDimensional : public Base {
  public:
    ThreeDimensional(SDL_Window* window, std::string_view app_name);
    ~ThreeDimensional();

    ThreeDimensional(const ThreeDimensional&) = delete;
    ThreeDimensional& operator=(const ThreeDimensional&) = delete;

    void DrawFrame() override;

  private:
    uint64_t frame_counter{0};

    static constexpr int FRAMES_IN_FLIGHT = 3;

    static constexpr uint64_t TIMEOUT_ONE_SECOND = 1000000000;

    vulkan::Instance instance;
    vulkan::DebugMessenger debug_messenger;
    vulkan::PhysicalDevice physical_device;
    vulkan::Device device;

    vulkan::Surface surface;
    vulkan::Swapchain swapchain;
    // assumed to also be the present queue
    vulkan::Queue graphics_queue;
    vulkan::FrameInFlightDataContainer<FRAMES_IN_FLIGHT> frame_in_flight_data;

    vulkan::ImmediateSubmit immediate_submit;

    vulkan::MemoryAllocator allocator;

    vulkan::AllocatedImage draw_image;

    imgui::Context imgui_context;

    vulkan::descriptors::Manager descriptor_manager;

    vulkan::pipelines::Manager pipeline_manager;

    void ImmediateSubmit(std::function<void(vk::CommandBuffer command_buffer)>&& function);

    void DrawBackground(vk::CommandBuffer command_buffer);
    void DrawGeometry(vk::CommandBuffer command_buffer);

    vulkan::FrameInFlightData& GetCurrentFrameInFlightData();

    vk::Queue GetQueue(vk::QueueFlags queue_flags);
};

}  // namespace jengine::renderer
