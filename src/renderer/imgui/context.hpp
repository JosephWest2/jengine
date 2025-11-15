#pragma once

#include <vulkan/vulkan_core.h>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "renderer/vulkan/deletion_stack.hpp"

namespace jengine::renderer::imgui {
class Context {
  public:
    Context(SDL_Window* window,
            VkDevice device,
            VkInstance instance,
            VkPhysicalDevice physical_device,
            VkQueue queue,
            uint32_t queue_family_index,
            VkFormat* swapchain_format_ptr,
            DeletionStack& deletion_stack);
    void NewFrame();

  private:
    static bool EventFilter(void* user_data, SDL_Event* event);
};
};  // namespace jengine::renderer::imgui
