#include "swapchain.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/initializers.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

Swapchain::Swapchain(uint width,
                     uint height,
                     vulkan::PhysicalDevice& physical_device,
                     const vk::raii::Device& device,
                     const vk::SurfaceKHR& surface)
    : swapchain(nullptr) {
    const auto& pd = physical_device.GetPhysicalDevice();

    auto surface_capabilities = pd.getSurfaceCapabilitiesKHR(surface);
    auto surface_formats = pd.getSurfaceFormatsKHR(surface);
    auto present_modes = pd.getSurfacePresentModesKHR(surface);

    surface_format = SelectSurfaceFormat(
        {.format = vk::Format::eB8G8R8A8Unorm, .colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear}, surface_formats);
    auto present_mode = SelectPresentMode(vk::PresentModeKHR::eFifo, present_modes);

    extent = vk::Extent2D{
        .width =
            std::clamp(width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width),
        .height =
            std::clamp(height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height),
    };

    std::vector<uint32_t> queue_family_indices;
    queue_family_indices.push_back(physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics));
    if (physical_device.GetPresentQueueIndex(surface) !=
        physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics)) {
        queue_family_indices.push_back(physical_device.GetPresentQueueIndex(surface));

        // logic works up till here, but synchronization needs to be implemented for drawing
        throw std::runtime_error(
            "Present queue family index does not match graphics queue family index, currently unsupported");
    }

    VkSurfaceKHR s = surface;
    vk::SwapchainCreateInfoKHR swapchain_create_info{
        .surface = s,

        .minImageCount = std::clamp(surface_capabilities.minImageCount + 1,
                                    surface_capabilities.minImageCount,
                                    surface_capabilities.maxImageCount),
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode =
            queue_family_indices.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,

        .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
        .pQueueFamilyIndices = queue_family_indices.data(),

        .preTransform = surface_capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,

        .presentMode = present_mode,
    };

    swapchain = device.createSwapchainKHR(swapchain_create_info);
    images = swapchain.getImages();

    for (auto& image : images) {
        vk::ImageViewCreateInfo image_view_create_info =
            init::ImageViewCreateInfo(image, surface_format.format, vk::ImageAspectFlagBits::eColor);
        image_views.push_back(device.createImageView(image_view_create_info));
        image_render_finished_semaphores.push_back(vk::raii::Semaphore(device, vk::SemaphoreCreateInfo{}));
    }
};

vk::SurfaceFormatKHR Swapchain::SelectSurfaceFormat(vk::SurfaceFormatKHR preferred,
                                                    std::vector<vk::SurfaceFormatKHR> const& available_formats) {
    std::optional<size_t> format_match_index = std::nullopt;
    std::optional<size_t> color_match_index = std::nullopt;
    for (size_t i = 0; i < available_formats.size(); i++) {
        auto& sf = available_formats[i];
        if (sf.format == preferred.format) {
            format_match_index = i;
        }
        if (sf.colorSpace == preferred.colorSpace) {
            color_match_index = i;
        }
        if (format_match_index && color_match_index && format_match_index.value() == color_match_index.value()) {
            return available_formats[format_match_index.value()];
        }
    }
    if (format_match_index) {
        return available_formats[format_match_index.value()];
    }
    if (color_match_index) {
        return available_formats[color_match_index.value()];
    }
    return available_formats[0];
}
vk::PresentModeKHR Swapchain::SelectPresentMode(vk::PresentModeKHR preferred,
                                                std::vector<vk::PresentModeKHR> const& available_present_modes) {
    if (std::ranges::any_of(available_present_modes, [&](auto mode) { return mode == preferred; })) {
        return preferred;
    }
    return vk::PresentModeKHR::eFifo;
}

void Swapchain::Rebuild(uint width,
                        uint height,
                        vulkan::PhysicalDevice& physical_device,
                        const vk::raii::Device& device,
                        const vk::SurfaceKHR& surface) {
    device.waitIdle();
    this->~Swapchain();
    new (this) Swapchain(width, height, physical_device, device, surface);
}
}  // namespace jengine::renderer::vulkan
