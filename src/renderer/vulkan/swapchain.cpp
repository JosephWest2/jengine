#include "swapchain.hpp"
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <stdexcept>
#include "VkBootstrap.h"
#include "renderer/vulkan/initializers.hpp"

namespace jengine::renderer::vulkan {


Swapchain::Swapchain(uint width, uint height, VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface) {
    if (!physical_device) {
        throw std::runtime_error("Physical device is null");
    }
    if (!device) {
        throw std::runtime_error("Device is null");
    }
    if (!surface) {
        throw std::runtime_error("Surface is null");
    }
    vkb::SwapchainBuilder swapchain_builder(physical_device, device, surface);

    swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

    auto swapchain_res = swapchain_builder
                             .set_desired_format(VkSurfaceFormatKHR{.format = swapchain_image_format,
                                                                    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                             .set_desired_present_mode(VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                             .set_desired_extent(width, height)
                             .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                             .build();

    if (!swapchain_res.has_value()) {
        throw std::runtime_error(std::format("Failed to create Vulkan swapchain: {}", swapchain_res.error().message()));
    }

    swapchain = swapchain_res->swapchain;
    extent = swapchain_res->extent;
    swapchain_images = swapchain_res->get_images().value();
    swapchain_image_views = swapchain_res->get_image_views().value();

    VkSemaphoreCreateInfo semaphore_create_info = vulkan::init::SemaphoreCreateInfo(0);
    image_render_finished_semaphores.resize(swapchain_images.size());

    for (int i = 0; i < swapchain_images.size(); i++) {
        if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_render_finished_semaphores[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create render finished semaphore");
        }
    }
};

void Swapchain::Destroy(VkDevice device) {
    assert(swapchain);
    assert(device);
    std::cout << "Destroying swapchain" << std::endl;
    for (auto view : swapchain_image_views) {
        vkDestroyImageView(device, view, nullptr);
    }
    for (auto image_semaphore : image_render_finished_semaphores) {
        vkDestroySemaphore(device, image_semaphore, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}
}  // namespace jengine::renderer::vulkan
