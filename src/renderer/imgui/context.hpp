#pragma once

#include <vulkan/vulkan_core.h>

#include <functional>
#include <stack>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

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
            std::stack<std::function<void()>>& deletion_stack);
  private:
    static bool EventFilter(void* user_data, SDL_Event* event);
};
};  // namespace jengine::renderer::imgui
