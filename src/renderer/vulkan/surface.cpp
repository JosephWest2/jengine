#include "surface.hpp"

#include <format>
#include <iostream>

#include "SDL3/SDL_vulkan.h"

namespace jengine::renderer::vulkan {

Surface::Surface(SDL_Window* window, VkInstance& instance): instance(instance) {
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        throw std::runtime_error(std::format("SDL_Vulkan_CreateSurface failed: {}", SDL_GetError()));
    }
}
Surface::~Surface() {
    std::cout << "Destroying surface" << std::endl;
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
}  // namespace jengine::renderer::vulkan
