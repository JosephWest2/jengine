#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>
#include "SDL3/SDL_video.h"
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan {
class Surface {
  public:
    Surface(SDL_Window* window, vk::raii::Instance& instance);
    ~Surface();

    vk::SurfaceKHR GetSurface() const { return surface; }

  private:
    vk::SurfaceKHR surface;

    // Held for destruction
    vk::raii::Instance& instance;
};
}  // namespace jengine::renderer::vulkan
