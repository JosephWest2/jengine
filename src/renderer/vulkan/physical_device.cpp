#include "physical_device.hpp"
#include "VkBootstrap.h"
#include <format>

namespace jengine::renderer::vulkan {

PhysicalDevice::PhysicalDevice(VkSurfaceKHR surface, vkb::Instance& instance) {

    VkPhysicalDeviceVulkan13Features vulkan_1_3_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    VkPhysicalDeviceVulkan12Features vulkan_1_2_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .bufferDeviceAddress = true,
    };

    vkb::PhysicalDeviceSelector physical_device_selector(instance, surface);

    auto physical_device_res = physical_device_selector.set_minimum_version(1, 3)
                                   .set_required_features_13(vulkan_1_3_features)
                                   .set_required_features_12(vulkan_1_2_features)
                                   .select();

    if (!physical_device_res.has_value()) {
        throw std::runtime_error(
            std::format("Failed to select Vulkan physical device: {}", physical_device_res.error().message()));
    }
    vkb_physical_device = physical_device_res.value();

}
}  // namespace jengine::renderer::vulkan
