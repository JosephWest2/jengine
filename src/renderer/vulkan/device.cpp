#include "device.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/physical_device.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

Device::Device(vulkan::PhysicalDevice& physical_device) : device(CreateDevice(physical_device)) {}

vk::raii::Device Device::CreateDevice(vulkan::PhysicalDevice& physical_device) {
    float queue_priority = 1.0f;
    vk::DeviceQueueCreateInfo queue_create_info = {
        .queueFamilyIndex = physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics),
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    vk::PhysicalDeviceVulkan13Features vulkan13_features{
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    vk::PhysicalDeviceVulkan12Features vulkan12_features{
        .pNext = &vulkan13_features,
        .descriptorIndexing = true,
        .bufferDeviceAddress = true,
    };
    vk::DeviceCreateInfo device_create_info{};
    device_create_info.pNext = &vulkan12_features;
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    device_create_info.ppEnabledExtensionNames = extensions.data();
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;

    return vk::raii::Device(physical_device.GetPhysicalDevice(), device_create_info);
}

}  // namespace jengine::renderer::vulkan
