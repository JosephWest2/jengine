#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <functional>

#include "SDL3/SDL_video.h"
#include "renderer/base.hpp"
#include "renderer/imgui/context.hpp"
#include "renderer/vulkan/descriptors/manager.hpp"
#include "renderer/vulkan/device.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/graphics_queue.hpp"
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
    ThreeDimensional(SDL_Window* window, vulkan::Instance& instance);
    ~ThreeDimensional();

    ThreeDimensional(const ThreeDimensional&) = delete;
    ThreeDimensional& operator=(const ThreeDimensional&) = delete;

    void DrawFrame() override;

  private:
    uint64_t frame_counter{0};

    static constexpr int FRAMES_IN_FLIGHT = 3;

    // instance must outlive the renderer
    vulkan::Instance& instance;

    vulkan::Surface surface;
    vulkan::PhysicalDevice physical_device;
    vulkan::Device device;
    vulkan::Swapchain swapchain;
    vulkan::GraphicsQueue graphics_queue;
    vulkan::FrameInFlightDataContainer<3> frame_in_flight_data;

    vulkan::GraphicsQueue graphics_queue_2;
    vulkan::ImmediateSubmit immediate_submit;

    vulkan::MemoryAllocator allocator;

    vulkan::AllocatedImage draw_image;

    imgui::Context imgui_context;

    vulkan::descriptors::Manager descriptor_manager;

    vulkan::pipelines::Manager pipeline_manager;

    void ImmediateSubmit(std::function<void(VkCommandBuffer command_buffer)>&& function);

    void DrawBackground(VkCommandBuffer command_buffer);

    vulkan::FrameInFlightData& GetCurrentFrameInFlightData();
};

}  // namespace jengine::renderer
