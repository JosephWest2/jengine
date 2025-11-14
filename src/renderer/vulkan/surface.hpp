#pragma once

#include <vulkan/vulkan_core.h>
#include "SDL3/SDL_video.h"
#include "deletion_stack.hpp"
namespace jengine::renderer::vulkan {
class Surface {
  public:
    Surface(SDL_Window* window, VkInstance instance, DeletionStack deletion_stack);

    VkSurfaceKHR GetSurface() const { return surface; }

    void Destroy(VkInstance instance);

  private:
    VkSurfaceKHR surface{};
};
}  // namespace jengine::renderer::vulkan
