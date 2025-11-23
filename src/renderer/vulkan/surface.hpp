#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

#include "SDL3/SDL_video.h"
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan {
class Surface {
  public:
    Surface(SDL_Window* window, const vk::Instance& instance);
    ~Surface();

    vk::SurfaceKHR& GetSurface() { return surface; }

  private:
    vk::SurfaceKHR surface;

    // Held for destruction
    const vk::Instance& instance;
};
}  // namespace jengine::renderer::vulkan
