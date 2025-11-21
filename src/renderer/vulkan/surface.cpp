#include "surface.hpp"

#include <format>

#include "SDL3/SDL_vulkan.h"

namespace jengine::renderer::vulkan {

Surface::Surface(SDL_Window* window, vk::raii::Instance& instance): instance(instance) {
    VkSurfaceKHR s;
    if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &s)) {
        throw std::runtime_error(std::format("SDL_Vulkan_CreateSurface failed: {}", SDL_GetError()));
    }
    surface = s;
}
Surface::~Surface() {
    SDL_Vulkan_DestroySurface(instance, surface, nullptr);
}
}  // namespace jengine::renderer::vulkan
