#pragma once

#include <sys/types.h>
#include <functional>
#include <stack>
#include <typeindex>
#include "SDL3/SDL_video.h"
#include "renderer/base.hpp"
#include "renderer/vulkan/deletion_queue.hpp"
#include "renderer/vulkan/device.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/instance.hpp"
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

    vulkan::DeletionQueue deletion_queue{};

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

    void Cleanup();
    void DrawFrame() override;
    vulkan::FrameInFlightData& GetCurrentFrameInFlightData() {
        return frame_in_flight_data[frame_counter % frame_in_flight_data.size()];
    }

};

}  // namespace jengine::renderer
