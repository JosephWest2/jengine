#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include <vulkan/vulkan_raii.hpp>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::imgui {
class Context {
  public:
    Context(SDL_Window* window,
            const vk::raii::Device& device,
            const vk::Instance& instance,
            const vk::PhysicalDevice& physical_device,
            const vk::Queue& queue,
            uint32_t queue_family_index,
            const vk::Format swapchain_format);
    ~Context();
    void NewFrame(std::function<void()>&& fn);
    void Draw(const vk::CommandBuffer& command_buffer, const vk::ImageView& target_image_view, const vk::Extent2D& swapchain_extent);

  private:
    static bool EventFilter(void* user_data, SDL_Event* event);

    vk::raii::DescriptorPool descriptor_pool;

};
};  // namespace jengine::renderer::imgui
