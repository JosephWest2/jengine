#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#include "SDL3/SDL_video.h"
#include "renderer/base.hpp"
#include "renderer/vulkan/device.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/memory_allocator.hpp"
#include "renderer/vulkan/physical_device.hpp"
#include "renderer/vulkan/surface.hpp"
#include "renderer/vulkan/swapchain.hpp"

namespace jengine::renderer {

inline constexpr int FRAMES_IN_FLIGHT = 3;

class ThreeDimensional : public Base {
  public:
    ThreeDimensional(SDL_Window* window, vulkan::Instance& instance);
    ~ThreeDimensional();

    ThreeDimensional(const ThreeDimensional&) = delete;
    ThreeDimensional& operator=(const ThreeDimensional&) = delete;

  private:

    uint64_t frame_counter{0};

    // instance must outlive the renderer
    vulkan::Instance& instance;

    vulkan::Surface surface;
    vulkan::PhysicalDevice physical_device;
    vulkan::Device device;
    vulkan::Swapchain swapchain;
    vulkan::GraphicsQueue graphics_queue;
    vulkan::FrameInFlightDataContainer<3> frame_in_flight_data;

    vulkan::MemoryAllocator allocator;

    vulkan::AllocatedImage draw_image;

    void Destroy();
    void DrawFrame() override;
    void DrawBackground(VkCommandBuffer command_buffer);
    vulkan::FrameInFlightData& GetCurrentFrameInFlightData() {
        return frame_in_flight_data[frame_counter % frame_in_flight_data.Size()];
    }

};

}  // namespace jengine::renderer
