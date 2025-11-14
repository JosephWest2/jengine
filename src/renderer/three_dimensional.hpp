#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#include <functional>
#include <stack>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "deletion_stack.hpp"
#include "renderer/base.hpp"
#include "renderer/imgui/context.hpp"
#include "renderer/vulkan/compute_queue.hpp"
#include "renderer/vulkan/descriptors.hpp"
#include "renderer/vulkan/device.hpp"
#include "renderer/vulkan/frame_in_flight_data.hpp"
#include "renderer/vulkan/graphics_queue.hpp"
#include "renderer/vulkan/image.hpp"
#include "renderer/vulkan/immediate_submit.hpp"
#include "renderer/vulkan/instance.hpp"
#include "renderer/vulkan/memory_allocator.hpp"
#include "renderer/vulkan/physical_device.hpp"
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

    static constexpr int MAX_DESCRIPTOR_SETS = 10;
    static constexpr int FRAMES_IN_FLIGHT = 3;

    // instance must outlive the renderer
    DeletionStack engine_lifetime_deletion_stack;
    vulkan::Instance& instance;

    vulkan::Surface surface;
    vulkan::PhysicalDevice physical_device;
    vulkan::Device device;
    vulkan::Swapchain swapchain;
    vulkan::GraphicsQueue graphics_queue;
    vulkan::FrameInFlightDataContainer<3> frame_in_flight_data;

    vulkan::ComputeQueue compute_queue;
    vulkan::ImmediateSubmit immediate_submit;

    vulkan::MemoryAllocator allocator;

    vulkan::AllocatedImage draw_image{};

    vulkan::DescriptorAllocator global_descriptor_set_allocator{};
    imgui::Context imgui_context;

    VkDescriptorSet draw_image_descriptors{};
    VkDescriptorSetLayout draw_image_descriptor_layout{};

    VkPipeline gradient_pipeline{};
    VkPipelineLayout gradient_pipeline_layout{};

    // should only be used for engine lifetime destruction
    std::stack<std::function<void()>> deletion_stack{};

    // imgui
    void InitImgui(SDL_Window* window);
    static bool ImguiSDLEventFilter(void* user_data, SDL_Event* event);

    void ImmediateSubmit(std::function<void(VkCommandBuffer command_buffer)>&& function);

    void Destroy();
    void DrawBackground(VkCommandBuffer command_buffer);
    vulkan::FrameInFlightData& GetCurrentFrameInFlightData();
    void InitDescriptors();
    void InitPipelines();
    void InitBackgroundPipleines();

};

}  // namespace jengine::renderer
