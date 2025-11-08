#pragma once

#include <vulkan/vulkan_core.h>
#include "SDL3/SDL_video.h"
namespace jengine::renderer::vulkan {
class Surface {
  public:
    Surface(SDL_Window* window, VkInstance instance);

    VkSurfaceKHR GetSurface() const { return surface; }

    void Destroy(VkInstance instance);

  private:
    VkSurfaceKHR surface{};
};
}  // namespace jengine::renderer::vulkan
