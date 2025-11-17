#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <vector>
namespace jengine::renderer::vulkan {

class Swapchain {
  public:
    Swapchain(uint width, uint height, VkPhysicalDevice physical_device, VkDevice& device, VkSurfaceKHR surface);
    ~Swapchain();

    VkFormat GetSwapchainImageFormat() const { return swapchain_image_format; }
    VkFormat* GetSwapchainImageFormatPtr() { return &swapchain_image_format; }
    VkSwapchainKHR GetSwapchain() const { return swapchain; }
    VkSwapchainKHR* GetSwapchainPtr() { return &swapchain; }
    const std::vector<VkSemaphore>& GetImageRenderFinishedSemaphores() const {
        return image_render_finished_semaphores;
    }
    const std::vector<VkImage>& GetSwapchainImages() const { return swapchain_images; }
    const std::vector<VkImageView>& GetSwapchainImageViews() const { return swapchain_image_views; }
    const VkExtent2D GetExtent() const { return extent; }

  private:
    VkExtent2D extent{};
    VkFormat swapchain_image_format{};
    VkSwapchainKHR swapchain{};
    std::vector<VkSemaphore> image_render_finished_semaphores{};
    std::vector<VkImage> swapchain_images{};
    std::vector<VkImageView> swapchain_image_views{};

    // held for destruction
    VkDevice& device;
};
}  // namespace jengine::renderer::vulkan
