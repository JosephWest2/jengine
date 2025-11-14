#include "surface.hpp"

#include <format>
#include <iostream>

#include "SDL3/SDL_vulkan.h"
#include "deletion_stack.hpp"

namespace jengine::renderer::vulkan {

Surface::Surface(SDL_Window* window, VkInstance instance, DeletionStack deletion_stack) {
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        throw std::runtime_error(std::format("SDL_Vulkan_CreateSurface failed: {}", SDL_GetError()));
    }

    deletion_stack.Push([this, instance]() { Destroy(instance); });
}
void Surface::Destroy(VkInstance instance) {
    std::cout << "Destroying surface" << std::endl;
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
}  // namespace jengine::renderer::vulkan
