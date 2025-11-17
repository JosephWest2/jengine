#pragma once

#include <vulkan/vulkan_core.h>
#include "SDL3/SDL_video.h"
namespace jengine::renderer::vulkan {
class Surface {
  public:
    Surface(SDL_Window* window, VkInstance& instance);
    ~Surface();

    VkSurfaceKHR GetSurface() const { return surface; }

  private:
    VkSurfaceKHR surface{};

    // Held for destruction
    VkInstance& instance;
};
}  // namespace jengine::renderer::vulkan
