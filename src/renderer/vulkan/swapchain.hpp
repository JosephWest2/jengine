#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/physical_device.hpp"
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan {

class Swapchain {
  public:
    Swapchain(uint width,
              uint height,
              vulkan::PhysicalDevice& physical_device,
              const vk::raii::Device& device,
              const vk::SurfaceKHR& surface);

    vk::Format GetSwapchainImageFormat() const { return surface_format.format; }
    const vk::Format* GetSwapchainImageFormatPtr() const { return &surface_format.format; }
    const vk::SwapchainKHR& GetSwapchain() const { return *swapchain; }
    const vk::SwapchainKHR* GetSwapchainPtr() const { return &*swapchain; }
    std::vector<vk::raii::Semaphore>& GetImageRenderFinishedSemaphores() {
        return image_render_finished_semaphores;
    }
    const std::vector<vk::Image>& GetSwapchainImages() const { return images; }
    const std::vector<vk::raii::ImageView>& GetSwapchainImageViews() const { return swapchain_image_views; }
    const vk::Extent2D& GetExtent() const { return extent; }

  private:
    vk::SurfaceFormatKHR surface_format{};
    vk::Extent2D extent{};
    vk::raii::SwapchainKHR swapchain;
    std::vector<vk::Image> images{};
    std::vector<vk::raii::ImageView> image_views{};
    std::vector<vk::raii::Semaphore> image_render_finished_semaphores{};
    std::vector<vk::raii::ImageView> swapchain_image_views{};

    static vk::SurfaceFormatKHR SelectSurfaceFormat(vk::SurfaceFormatKHR preferred,
                                                    std::vector<vk::SurfaceFormatKHR> const& available_formats);
    static vk::PresentModeKHR SelectPresentMode(vk::PresentModeKHR preferred,
                                                std::vector<vk::PresentModeKHR> const& available_present_modes);
};
}  // namespace jengine::renderer::vulkan
