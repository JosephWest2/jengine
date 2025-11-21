#include "device.hpp"

#include "renderer/vulkan/physical_device.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {

Device::Device(vulkan::PhysicalDevice& physical_device) : device(CreateDevice(physical_device)) {}

vk::raii::Device Device::CreateDevice(vulkan::PhysicalDevice& physical_device) {
    vk::DeviceQueueCreateInfo queue_create_info = {
        .queueFamilyIndex = physical_device.GetQueueFamilyIndex(vk::QueueFlagBits::eGraphics),
        .queueCount = 1,
    };

    vk::DeviceCreateInfo device_create_info{};
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;

    return vk::raii::Device(physical_device.GetPhysicalDevice(), device_create_info);
}

}  // namespace jengine::renderer::vulkan
