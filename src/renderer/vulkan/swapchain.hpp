#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <vector>
#include "SDL3/SDL_video.h"
namespace jengine::renderer::vulkan {

class Swapchain {
  public:
    Swapchain(uint width, uint height, VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface);
    Swapchain(SDL_Window* window, VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface);

    // does not call device wait idle
    void Destroy(VkDevice device);

    VkFormat GetSwapchainImageFormat() const { return swapchain_image_format; }
    VkSwapchainKHR GetSwapchain() const { return swapchain; }
    VkSwapchainKHR* GetSwapchainPtr() { return &swapchain; }
    const std::vector<VkSemaphore>& GetImageRenderFinishedSemaphores() const {
        return image_render_finished_semaphores;
    }
    const std::vector<VkImage>& GetSwapchainImages() const { return swapchain_images; }
    const std::vector<VkImageView>& GetSwapchainImageViews() const { return swapchain_image_views; }

  private:
    VkFormat swapchain_image_format{};
    VkSwapchainKHR swapchain{};
    std::vector<VkSemaphore> image_render_finished_semaphores{};
    std::vector<VkImage> swapchain_images{};
    std::vector<VkImageView> swapchain_image_views{};
};
}  // namespace jengine::renderer::vulkan
