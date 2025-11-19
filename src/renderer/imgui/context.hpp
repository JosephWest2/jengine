#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

namespace jengine::renderer::imgui {
class Context {
  public:
    Context(SDL_Window* window,
            VkDevice& device,
            VkInstance instance,
            VkPhysicalDevice physical_device,
            VkQueue queue,
            uint32_t queue_family_index,
            VkFormat* swapchain_format_ptr);
    ~Context();
    void NewFrame(std::function<void()> fn);
    void Draw(VkCommandBuffer command_buffer, VkImageView target_image_view, VkExtent2D swapchain_extent);

  private:
    static bool EventFilter(void* user_data, SDL_Event* event);

    VkDescriptorPool descriptor_pool{};

    // held for destruction
    VkDevice& device;
};
};  // namespace jengine::renderer::imgui
